#include "cluster/em_clusterer.hpp"

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
void em_clusterer<Estimator, FeatureSelector>::add_cluster(
    const std::string & uid)
{
    vector<string>::iterator c_it = lower_bound(
        _clusters.begin(), _clusters.end(), uid);

    if(c_it != _clusters.end() && *c_it == uid)
        throw runtime_error("duplicate cluster UID " + uid);

    unsigned ind = distance(_clusters.begin(), c_it);

    _clusters.insert(_clusters.begin() + ind, uid);
    _estimators.insert(_estimators.begin() + ind,
        typename Estimator::ptr_t(new Estimator()));

    for(typename samples_t::iterator s_it = _samples.begin();
        s_it != _samples.end(); ++s_it)
    {
        sample_t & sample(s_it->second);

        typename sample_t::prob_t prob;
        prob.sample_class = 0;
        prob.class_sample = 0;

        sample.prob.insert(sample.prob.begin() + ind, prob);
        sample.hard.insert(sample.hard.begin() + ind, false);
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

        sample.prob.erase(sample.prob.begin() + ind);
        sample.hard.erase(sample.hard.begin() + ind);
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
    sample.features = feat;

    typename sample_t::prob_t prob;
    prob.sample_class = 0;
    prob.class_sample = 0;

    // init probability state
    sample.prob.resize(_clusters.size(), prob);
    sample.hard.resize(_clusters.size(), false);

    // iterate over (cluster-uid, (P(class | sample), is-hard))
    for(sample_cluster_state_t::const_iterator s_it = cluster_probs.begin();
        s_it != cluster_probs.end(); ++s_it)
    {
        vector<string>::iterator c_it = lower_bound(
            _clusters.begin(), _clusters.end(), s_it->first);

        if(c_it == _clusters.end() || *c_it != s_it->first)
            throw runtime_error("no cluster with UID " + s_it->first);

        unsigned ind = distance(_clusters.begin(), c_it);

        sample.prob[ind].class_sample = s_it->second.first;
        sample.hard[ind] = s_it->second.second;
    }
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

    double hard_norm, soft_norm;
    compute_prob_norms(sample, hard_norm, soft_norm);

    sample_cluster_state_t probs;

    vector<string>::const_iterator it1 = _clusters.begin();
    typename sample_t::probs_t::const_iterator it2 = sample.prob.begin();
    vector<bool>::const_iterator it3 = sample.hard.begin();

    for(; it1 != _clusters.end(); ++it1, ++it2, ++it3)
    {
        probs[*it1] = make_pair(
            it2->class_sample * (
                *it3 ? hard_norm : soft_norm), *it3);
    }
    return probs;
}

template<
    typename Estimator,
    typename FeatureSelector
>
void em_clusterer<Estimator, FeatureSelector>::compute_prob_norms(
    const sample_t & sample,
    double & hard_norm, double & soft_norm)
{
    // compute sum{ P(class | sample) } (for normalization)
    soft_norm = 0; hard_norm = 0;
    for(size_t i = 0; i != sample.prob.size(); ++i)
    {
        if(sample.hard[i])
            hard_norm += sample.prob[i].class_sample;
        else
            soft_norm += sample.prob[i].class_sample;
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
    return;
}

template<
    typename Estimator,
    typename FeatureSelector
>
double em_clusterer<Estimator, FeatureSelector>::iterate()
{
    unsigned num_clusters = _clusters.size();

    for(unsigned i = 0; i != num_clusters; ++i)
        _estimators[i]->reset();

    vector<double> cluster_probs(num_clusters, 0);

    for(typename samples_t::const_iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        const sample_t & sample( it->second);

        double hard_norm, soft_norm;
        compute_prob_norms(sample, hard_norm, soft_norm);

        // feed features & P(class | sample) to estimators
        for(size_t i = 0; i != num_clusters; ++i)
        {
            double prob_class_sample = sample.prob[i].class_sample * (
                sample.hard[i] ? hard_norm : soft_norm);

            _estimators[i]->add_observation(
                sample.features, prob_class_sample);

            // P(c) = sum{ P(c|s) for s in S}
            cluster_probs[i] += prob_class_sample;
        }
    }

    double cluster_pnorm = 1.0 / std::accumulate(
        cluster_probs.begin(), cluster_probs.end(), 0);

    for(size_t i = 0; i != num_clusters; ++i)
    {
        cluster_probs[i] *= cluster_pnorm;

        // shift probability slightly back to even
        double avg_cluster_prob = 1.0 / num_clusters;
        cluster_probs[i] += 0.02 * (avg_cluster_prob - cluster_probs[i]);

        _estimators[i]->prepare_estimator();
    }

    for(typename samples_t::iterator it = _samples.begin();
        it != _samples.end(); ++it)
    {
        sample_t & sample( it->second);

        double lprob_norm = -DBL_MAX;
        for(size_t i = 0; i != num_clusters; ++i)
        {
            // query estimator for log P(sample|class)
            sample.prob[i].sample_class = \
                _estimators[i]->estimate(sample.features);

            // track largest log P(sample | class)
            // TODO: Want c++0x lambdas!
            if(sample.prob[i].sample_class > lprob_norm)
                lprob_norm = sample.prob[i].sample_class;
        }

        double prob_sample = 0;
        for(size_t i = 0; i != num_clusters; ++i)
        {
            // for numeric stability, subtract (divide in log space)
            //  a constant factor, which preserves relative probabilities
            //  but shifts the largest probability up to 1
            sample.prob[i].sample_class = std::exp(
                sample.prob[i].sample_class - lprob_norm);

            prob_sample += sample.prob[i].sample_class;
        }

        for(size_t i = 0; i != num_clusters; ++i)
        {
            if(sample.hard[i])
                continue;

            // P(class|sample) = P(class) * P(sample|class) / P(sample)
            sample.prob[i].class_sample = \
                cluster_probs[i] * sample.prob[i].sample_class / prob_sample;
        }
    }
    return 0;
}

};

