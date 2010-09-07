
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
void em_clusterer<InputFeatures, Estimator>::sample_t::norm_class_probs()
{
    // compute sum{ P(class | sample) } (for normalization)
    double soft_norm = 0;
    double hard_norm = 0;

    for(unsigned i = 0; i != prob_class_sample.size(); ++i)
    {
        if(is_hard[i])
            hard_norm += prob_class_sample[i];
        else
            soft_norm += prob_class_sample[i];
    }

    if(soft_norm && hard_norm < 1)
    {
        soft_norm = (1 - hard_norm) / soft_norm;
        hard_norm = 1;
    }
    else if(hard_norm)
    {
        soft_norm = 0;
        hard_norm = 1 / hard_norm;
    }
    else
    {
        soft_norm = hard_norm = 1;
    }

    for(unsigned i = 0; i != prob_class_sample.size(); ++i)
        prob_class_sample[i] *= is_hard[i] ? hard_norm : soft_norm;

    return;
}


template<typename InputFeatures, typename Estimator>
void em_clusterer<InputFeatures, Estimator>::add_cluster(
    const std::string & uid, typename Estimator::ptr_t estimator)
{
    vector<string>::iterator c_it = lower_bound(
        _clusters.begin(), _clusters.end(), uid);

    if(c_it != _clusters.end() && *c_it == uid)
        throw runtime_error("duplicate cluster UID " + uid);

    unsigned ind = distance(_clusters.begin(), c_it);

    _clusters.insert(_clusters.begin() + ind,     uid);
    _estimators.insert(_estimators.begin() + ind, estimator);

    for(typename samples_t::iterator s_it = _samples.begin();
        s_it != _samples.end(); ++s_it)
    {
        sample_t & sample(s_it->second);

        // no need to normalize cluster probs (we're adding 0)
        sample.prob_sample_class.insert(
            sample.prob_sample_class.begin() + ind, 0);
        sample.prob_class_sample.insert(
            sample.prob_class_sample.begin() + ind, 0);
        sample.is_hard.insert(
            sample.is_hard.begin() + ind, false);
    }
    return;
}

template<typename InputFeatures, typename Estimator>
void em_clusterer<InputFeatures, Estimator>::drop_cluster(
    const string & uid)
{
    vector<string>::iterator c_it = lower_bound(
        _clusters.begin(), _clusters.end(), uid);

    if(c_it == _clusters.end() || *c_it != uid)
        throw runtime_error("no cluster with UID " + uid);

    unsigned ind = distance(_clusters.begin(), c_it);

    _clusters.erase(_clusters.begin() + ind);
    _estimators.erase(_estimators.begin() + ind);

    for(typename samples_t::iterator s_it = _samples.begin();
        s_it != _samples.end(); ++s_it)
    {
        sample_t & sample(s_it->second);

        sample.prob_sample_class.erase(sample.prob_sample_class.begin() + ind);
        sample.prob_class_sample.erase(sample.prob_class_sample.begin() + ind);
        sample.is_hard.erase(sample.is_hard.begin() + ind);

        // renormalize P(class | sample) to 1
        sample.norm_class_probs();
    }
    return;
}

// meta-classes for setting est_features iff
//  it's the same type as input_features
namespace {
    template<bool eq_feat_types>
    struct _est_features_init
    {
        template<typename Fi, typename Fe>
        void operator()(Fe & fe, const Fi & fi)
        { fe = fi; }
    };

    template<>
    struct _est_features_init<false>
    {
        template<typename Fi, typename Fe>
        void operator()(Fe & fe, const Fi & fi)
        { }
    };
};

template<typename InputFeatures, typename Estimator>
void em_clusterer<InputFeatures, Estimator>::add_sample(
    const string & uid,
    const typename input_features_t::ptr_t & input_feat,
    const sample_cluster_state_t & cluster_probs)
{
    if(_samples.find(uid) != _samples.end())
        throw runtime_error("duplicate sample UID " + uid);

    sample_t & sample( _samples[uid]);

    // init probability state
    sample.prob_class_sample.resize(_clusters.size(), 0);
    sample.prob_sample_class.resize(_clusters.size(), 0);
    sample.is_hard.resize(_clusters.size(), false);

    sample.input_features = input_feat;

    // iff types are the same, set est_features to input_features
    _est_features_init< boost::is_same<input_features_t, estimator_features_t
        >::value>()(sample.est_features, input_feat);

    // iterate over (cluster-uid, (P(class | sample), is-hard))
    for(sample_cluster_state_t::const_iterator s_it = cluster_probs.begin();
        s_it != cluster_probs.end(); ++s_it)
    {
        vector<string>::iterator c_it = lower_bound(
            _clusters.begin(), _clusters.end(), s_it->first);

        if(c_it == _clusters.end() || *c_it != s_it->first)
            throw runtime_error("no cluster with UID " + s_it->first);

        unsigned ind = distance(_clusters.begin(), c_it);

        sample.prob_class_sample[ind] = s_it->second.first;
        sample.is_hard[ind] = s_it->second.second;
    }

    sample.norm_class_probs();
    return;
}

template<typename InputFeatures, typename Estimator>
void em_clusterer<InputFeatures, Estimator>::drop_sample(
    const string & uid)
{
    typename samples_t::iterator s_it = _samples.find(uid);

    if(s_it == _samples.end())
        throw runtime_error("no sample with UID " + uid);

    _samples.erase(s_it);
    return;
}

template<typename InputFeatures, typename Estimator>
typename em_clusterer<InputFeatures, Estimator>::sample_cluster_state_t
em_clusterer<InputFeatures, Estimator>::get_sample_probabilities(
    const string & uid)
{
    if(_samples.find(uid) == _samples.end())
        throw runtime_error("no sample with UID " + uid);

    sample_t & sample = _samples[uid];

    sample_cluster_state_t probs;

    vector<string>::const_iterator it1 = _clusters.begin();
    vector<double>::const_iterator it2 = sample.prob_class_sample.begin();
    vector<bool>::const_iterator   it3 = sample.is_hard.begin();

    for(; it1 != _clusters.end(); ++it1, ++it2, ++it3)
    {
        probs[*it1] = make_pair(*it2, *it3);
    }
    return probs;
}

template<typename InputFeatures, typename Estimator>
typename em_clusterer<InputFeatures, Estimator>::estimator_features_t::ptr_t
em_clusterer<InputFeatures, Estimator>::get_estimator_features(
    const string & uid)
{
    typename samples_t::iterator it = _samples.find(uid);

    if(it == _samples.end())
        throw runtime_error("no sample with UID " + uid);

    return it->second.est_features;
}

template<typename InputFeatures, typename Estimator>
double em_clusterer<InputFeatures, Estimator>::get_sample_likelihood(
    const string & uid)
{
    typename samples_t::iterator it = _samples.find(uid);

    if(it == _samples.end())
        throw runtime_error("no sample with UID " + uid);

    return it->second.prob_sample;
}

template<typename InputFeatures, typename Estimator>
template<typename FeatureTransform>
unsigned em_clusterer<InputFeatures, Estimator>::transform_features(
    const typename FeatureTransform::ptr_t & feature_transform)
{
    // extractor of sample features from samples_t value
    boost::function<
        const typename InputFeatures::ptr_t & (
            const typename samples_t::value_type &)
    > feat_ex(
        boost::bind( &sample_t::input_features,
            boost::bind( &samples_t::value_type::second, boost::lambda::_1)));

    // extractor of sample probability from samples_t value
    boost::function<
        const std::vector<double> & (
            const typename samples_t::value_type &)
    > prob_ex(
        boost::bind( &sample_t::prob_class_sample,
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
        sample_t & sample(it->second);

        sample.est_features = do_transform(sample.input_features);
    }

    // return number of active features
    return 0;
}

template<typename InputFeatures, typename Estimator>
double em_clusterer<InputFeatures, Estimator>::expect_and_maximize()
{
    unsigned num_clusters = _clusters.size();

    for(unsigned i = 0; i != num_clusters; ++i)
        _estimators[i]->reset();

    // cluster priors P(c)
    vector<double> cluster_prob(num_clusters, 0);

    /// Expectation Step (part 1: training)
    {
        for(typename samples_t::const_iterator it = _samples.begin();
            it != _samples.end(); ++it)
        {
            const sample_t & sample( it->second);

            // feed features & P(class | sample) to estimators
            for(size_t i = 0; i != num_clusters; ++i)
            {
                _estimators[i]->add_observation(
                    sample.est_features,
                    sample.prob_class_sample[i],
                    sample.is_hard[i]);

                // while we're here, sum cluster mass
                // P(c) = sum{ P(c|s) for s in S}
                cluster_prob[i] += sample.prob_class_sample[i];
            }
        }
    }

    /// Expectation Step (part 2: cluster priors)
    {
        vector_ops::normalize_L1(cluster_prob);

        std::cout << "cluster priors: ";
        for(size_t i = 0; i != num_clusters; ++i)
        {
            // shift probability slightly back to even
            double avg_cluster_prob = 1.0 / num_clusters;
            cluster_prob[i] += 0.02 * (avg_cluster_prob - cluster_prob[i]);

            std::cout << cluster_prob[i] << ", ";
        }
        std::cout << std::endl;
    }

    double entropy = 0;
    for(unsigned i = 0; i != num_clusters; ++i)
        entropy += _estimators[i]->prepare_estimator();

    /// Maximization Step
    for(typename samples_t::iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        sample_t & sample( it->second);

        // track the total generative probability of the sample;
        //   useful for identifying the must unlikely samples
        //   under the current model, which might be good
        //   candidates for active-learning
        sample.prob_sample = 0;

        /// Maximization Step
        for(size_t i = 0; i != num_clusters; ++i)
        {
            // query estimator for P(sample|class)
            sample.prob_sample_class[i] = \
                _estimators[i]->estimate(sample.est_features);

            // P(class|sample) = P(sample|class) * P(class) / P(sample)
            double p_sample = sample.prob_sample_class[i] * cluster_prob[i];

            sample.prob_sample += p_sample;

            if(sample.is_hard[i])
                continue;

            sample.prob_class_sample[i] = p_sample;
        }
        sample.norm_class_probs();
    }

    return entropy;
}

};

