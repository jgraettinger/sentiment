
#ifndef CLUSTER_EM_CLUSTERER_HPP
#define CLUSTER_EM_CLUSTERER_HPP

#include <string>
#include <vector>
#include <map>

namespace cluster
{

template<
    typename Estimator,
    typename FeatureSelector
>
class em_clusterer
{
public:

    typedef typename Estimator::features_t features_t;
    typedef typename features_t::ptr_t features_ptr_t;

    typedef std::map<
        std::string,
        std::pair<double, bool>
    > sample_cluster_state_t;

    em_clusterer() {}

    void add_cluster(
        const std::string & cluster_uid);

    void drop_cluster(
        const std::string & cluster_uid);

    void add_sample(
        const std::string & uid,
        const features_ptr_t & feat,
        // {'cluster-uid': (P(c|s), is-hard)}
        const sample_cluster_state_t & cluster_probs
    );

    void drop_sample(const std::string & uid);

    sample_cluster_state_t get_sample_probabilities(
        const std::string & sample_uid);

    double iterate();

private:

    struct sample_t {

        struct prob_t
        {
            // P(sample | cluster)
            double sample_class;
            // P(cluster | sample)
            double class_sample;
        };

        typedef std::vector<prob_t> probs_t;

        features_ptr_t features;

        probs_t prob;

        // whether P(cluster|sample) is 'hard'
        std::vector<bool> hard;
    };

    typedef std::map<std::string, sample_t> samples_t;

    void compute_prob_norms(
        const sample_t & sample,
        double & hard_norm, double & soft_norm);

    std::vector<std::string> _clusters;
    std::vector<typename Estimator::ptr_t> _estimators;

    samples_t _samples;
};

};

#include "cluster/em_clusterer.impl.hpp"

#endif

