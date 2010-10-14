
#include "cluster/em_clusterer.hpp"
#include "vector_ops.hpp"

#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>
#include <cfloat>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>

#include <iostream>

namespace cluster
{

using namespace std;

template<typename InputFeatures, typename Estimator>
em_clusterer<InputFeatures, Estimator>::em_clusterer()
 : _cluster_set(new cluster_set())
{ }

template<typename InputFeatures, typename Estimator>
void em_clusterer<InputFeatures, Estimator>::add_cluster(
    const std::string & uid, typename Estimator::ptr_t estimator)
{
    unsigned ind = _cluster_set->add_cluster(uid);
    _estimators.insert(_estimators.begin() + ind, estimator);

    for(typename samples_t::iterator s_it = _samples.begin();
        s_it != _samples.end(); ++s_it)
    {
        s_it->second->notify_of_added_cluster(ind);
    }
    return;
}

template<typename InputFeatures, typename Estimator>
void em_clusterer<InputFeatures, Estimator>::drop_cluster(
    const string & uid)
{
    unsigned ind = _cluster_set->drop_cluster(uid);
    _estimators.erase(_estimators.begin() + ind);

    for(typename samples_t::iterator s_it = _samples.begin();
        s_it != _samples.end(); ++s_it)
    {
        s_it->second->notify_of_dropped_cluster(ind);
    }
    return;
}

template<typename InputFeatures, typename Estimator>
typename cluster_sample<InputFeatures, typename Estimator::features_t>::ptr_t
em_clusterer<InputFeatures, Estimator>::add_sample(
    const string & uid,
    const typename input_features_t::ptr_t & input_features)
{
    if(_samples.find(uid) != _samples.end())
        throw runtime_error("duplicate sample UID " + uid);

    typename cluster_sample_t::ptr_t sample(new cluster_sample_t(
        uid, input_features, _cluster_set));

    _samples[uid] = sample;
    return sample;
}

template<typename InputFeatures, typename Estimator>
typename cluster_sample<InputFeatures, typename Estimator::features_t>::ptr_t
em_clusterer<InputFeatures, Estimator>::get_sample(
    const string & uid)
{
    typename samples_t::iterator s_it = _samples.find(uid);
    if(s_it == _samples.end())
        throw runtime_error("no sample with UID " + uid);

    return s_it->second;
}

template<typename InputFeatures, typename Estimator>
boost::python::list em_clusterer<InputFeatures, Estimator>::py_samples()
{
    boost::python::list samples;
    for(typename samples_t::const_iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        samples.append(it->second);
    }
    return samples;
}

template<typename InputFeatures, typename Estimator>
void em_clusterer<InputFeatures, Estimator>::drop_sample(
    const string & uid)
{
    typename samples_t::iterator s_it = _samples.find(uid);
    if(s_it == _samples.end())
        throw runtime_error("no sample with UID " + uid);

    _samples.erase(s_it);
}

template<typename InputFeatures, typename Estimator>
template<typename FeatureTransform>
void em_clusterer<InputFeatures, Estimator>::transform_features(
    const typename FeatureTransform::ptr_t & feature_transform)
{
    // extractor of sample input features from samples_t value_type
    boost::function<
        const typename InputFeatures::ptr_t & (
            const typename samples_t::value_type &)
    > feat_ex(
        boost::bind( &cluster_sample_t::input_features,
            boost::bind( &samples_t::value_type::second, boost::lambda::_1)));

    // extractor of sample P(cluster|sample) vector from samples_t value_type
    boost::function<
        const std::vector<double> & (
            const typename samples_t::value_type &)
    > prob_ex(
        boost::bind( &cluster_sample_t::prob_cluster_sample,
            boost::bind( &samples_t::value_type::second, boost::lambda::_1)));

    // functors for training & performing the transform
    typename FeatureTransform::template train_transform<
        InputFeatures> train_transform(feature_transform);
    typename FeatureTransform::template transform<
        InputFeatures> do_transform(feature_transform);

    train_transform(
        boost::make_zip_iterator( boost::make_tuple(
            boost::make_transform_iterator(_samples.begin(), feat_ex),
            boost::make_transform_iterator(_samples.begin(), prob_ex))),
        boost::make_zip_iterator( boost::make_tuple(
            boost::make_transform_iterator(_samples.end(), feat_ex),
            boost::make_transform_iterator(_samples.end(), prob_ex))));

    for(typename samples_t::iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        cluster_sample_t & sample(*it->second);

        sample.est_features = do_transform(sample.input_features);
    }
}

template<typename InputFeatures, typename Estimator>
double em_clusterer<InputFeatures, Estimator>::expect_and_maximize()
{
    unsigned num_clusters = _cluster_set->size();

    for(unsigned i = 0; i != num_clusters; ++i)
        _estimators[i]->reset();

    // cluster priors P(c)
    std::vector<double> & cluster_priors(_cluster_set->priors);
    cluster_priors.clear();
    cluster_priors.resize(num_clusters, 0);

    /// Expectation Step (part 1: training)
    {
        for(typename samples_t::const_iterator it = _samples.begin();
            it != _samples.end(); ++it)
        {
            const cluster_sample_t & sample(*it->second);

            if(!sample.est_features)
                throw runtime_error("expect_and_maximize(): "\
                    "a feature transform must be run first");

            // feed features & P(cluster | sample) to estimators
            for(size_t i = 0; i != num_clusters; ++i)
            {
                // factor sample weight into effective cluster probability
                double p_cluster = sample.prob_cluster_sample[i] * sample.weight;

                // inform estimator of this observation
                _estimators[i]->add_observation(
                    sample.est_features, p_cluster, sample.is_hard[i]);

                // while we're here, sum effective cluster probability mass
                //   P(c) = sum{ P(c|s) for s in S}
                cluster_priors[i] += p_cluster;
            }
        }
    }

    /// Expectation Step (part 2: cluster priors)
    {
        vector_ops::normalize_L1(cluster_priors);

//        std::cout << "cluster priors: ";
        for(size_t i = 0; i != num_clusters; ++i)
        {
            // shift probability slightly back to even
            double avg_cluster_prob = 1.0 / num_clusters;
            cluster_priors[i] += 0.02 * (avg_cluster_prob - cluster_priors[i]);

//            std::cout << cluster_priors[i] << ", ";
        }
//        std::cout << std::endl;
    }

    double entropy = 0;
    for(unsigned i = 0; i != num_clusters; ++i)
        entropy += _estimators[i]->prepare_estimator();

    /// Maximization Step
    for(typename samples_t::iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        cluster_sample_t & sample(*it->second);

        // track the total generative probability of the sample;
        //   useful for identifying the must unlikely samples
        //   under the current model, which might be good
        //   candidates for active-learning
        sample.prob_sample = 0;

        /// Maximization Step
        for(size_t i = 0; i != num_clusters; ++i)
        {
            // query estimator for P(sample|cluster)
            sample.prob_sample_cluster[i] = \
                _estimators[i]->estimate(sample.est_features);

            // P(cluster & sample) = P(sample|cluster) * P(cluster)
            double p_sample = sample.prob_sample_cluster[i] * cluster_priors[i];

            sample.prob_sample += p_sample;

            if(sample.is_hard[i])
                continue;

            sample.prob_cluster_sample[i] = p_sample;
        }
        // P(cluster|sample) = P(sample & cluster) / P(sample)
        sample.norm_cluster_probs();
    }

    return entropy;
}

};

