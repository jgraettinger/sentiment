#include "cluster/em_clusterer.hpp"
#include "cluster/vector_ops.hpp"
#include <boost/unordered_set.hpp>
#include <cfloat>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>

#include <iostream>

namespace cluster
{
using namespace std;

template<
    typename Estimator,
    typename FeatureSelector
>
void em_clusterer<Estimator, FeatureSelector>::sample_t::norm_class_probs()
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


template<
    typename Estimator,
    typename FeatureSelector
>
void em_clusterer<Estimator, FeatureSelector>::add_cluster(
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

template<
    typename Estimator,
    typename FeatureSelector
>
void em_clusterer<Estimator, FeatureSelector>::drop_cluster(
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

template<
    typename Estimator,
    typename FeatureSelector
>
void em_clusterer<Estimator, FeatureSelector>::add_sample(
    const string & uid,
    const features_ptr_t & feat,
    const sample_cluster_state_t & cluster_probs)
{
    if(_samples.find(uid) != _samples.end())
        throw runtime_error("duplicate sample UID " + uid);

    sample_t & sample( _samples[uid]);

    // init probability state
    sample.prob_class_sample.resize(_clusters.size(), 0);
    sample.prob_sample_class.resize(_clusters.size(), 0);
    sample.is_hard.resize(_clusters.size(), false);

    sample.features = feat;
    sample.filtered_features = feat;

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

template<
    typename Estimator,
    typename FeatureSelector
>
void em_clusterer<Estimator, FeatureSelector>::drop_sample(
    const string & uid)
{
    typename samples_t::iterator s_it = _samples.find(uid);

    if(s_it == _samples.end())
        throw runtime_error("no sample with UID " + uid);

    _samples.erase(s_it);
    return;
}

template<
    typename Estimator,
    typename FeatureSelector
>
typename em_clusterer<Estimator, FeatureSelector>::sample_cluster_state_t
em_clusterer<Estimator, FeatureSelector>::get_sample_probabilities(
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

template<
    typename Estimator,
    typename FeatureSelector
>
unsigned em_clusterer<Estimator, FeatureSelector>::feature_selection()
{
    _feature_selector->reset();

    // prime feature-selection by passing current
    //  features & class membership observations
    for(typename samples_t::const_iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        const sample_t & sample(it->second);

        // feed features & P(class | sample) to feature-selector
        _feature_selector->add_observation(
            sample.features, sample.prob_class_sample);
    }

    unsigned feat_count = _feature_selector->prepare_selector();

    // generate filtered & normalized features for each sample
    for(typename samples_t::iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        sample_t & sample(it->second);

        sample.filtered_features = \
            _feature_selector->filter_features(sample.features);
    }

    // return number of active features
    return feat_count;
}

template<
    typename Estimator,
    typename FeatureSelector
>
double em_clusterer<Estimator, FeatureSelector>::expect_and_maximize()
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
                    sample.filtered_features, sample.prob_class_sample[i]);

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

    for(unsigned i = 0; i != num_clusters; ++i)
            _estimators[i]->prepare_estimator();

    for(typename samples_t::iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        sample_t & sample( it->second);

        /// Expectation Step (part 3: decode P(sample | class))
        {
            for(size_t i = 0; i != num_clusters; ++i)
            {
                // query estimator for log P(sample|class)
                sample.prob_sample_class[i] = \
                    _estimators[i]->estimate(sample.filtered_features);
            }

            // track the total generative probability of the sample;
            //  useful for identifying samples which are poorly
            //  explained by the current model / clusters
            sample.log_prob_sample = std::accumulate(
                sample.prob_sample_class.begin(),
                sample.prob_sample_class.end(), 0);

            // largest log P(sample | class)
            double lprob_norm = *std::max_element(
                sample.prob_sample_class.begin(),
                sample.prob_sample_class.end());

            // for numeric stability, subtract (divide in log space)
            //  a constant factor, which preserves relative probabilities
            //  but shifts the largest probability up to 1
            for(size_t i = 0; i != num_clusters; ++i)
            {
                sample.prob_sample_class[i] = std::exp(
                    sample.prob_sample_class[i] - lprob_norm);
            }

            // normalize to reflect P(sample | class) / P(sample)
            vector_ops::normalize_L1(sample.prob_sample_class);
        }

        /// Maximization Step
        for(size_t i = 0; i != num_clusters; ++i)
        {
            if(sample.is_hard[i])
                continue;

            // P(class|sample) = P(class) * P(sample|class) / P(sample)
            sample.prob_class_sample[i] = \
                cluster_prob[i] * sample.prob_sample_class[i];
        }
        sample.norm_class_probs();
    }
    return 0;
}


};

