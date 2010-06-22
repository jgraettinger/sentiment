
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

    em_clusterer(typename FeatureSelector::ptr_t feature_selector)
      : _feature_selector(feature_selector)
    { }

    void add_cluster(
        const std::string & cluster_uid,
        typename Estimator::ptr_t estimator);

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

    unsigned feature_selection();

    double expect_and_maximize();

private:

    struct sample_t {

        // P(sample | class)
        std::vector<double> prob_sample_class;

        // P(class | sample)
        std::vector<double> prob_class_sample;

        // whether P(cluster|sample) is 'hard'
        std::vector<bool> is_hard;

        features_ptr_t features;
        features_ptr_t filtered_features;

        // unnormalized sum{ P(sample | class) for class in C}
        double log_prob_sample;

        void norm_class_probs();
    };

    typedef std::map<std::string, sample_t> samples_t;

    std::vector<std::string> _clusters;
    std::vector<typename Estimator::ptr_t> _estimators;

    typename FeatureSelector::ptr_t _feature_selector;
    samples_t _samples;
};

};

#include "cluster/em_clusterer.impl.hpp"

#endif

