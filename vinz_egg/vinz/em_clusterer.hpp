
#ifndef EM_CLUSTERER_HPP
#define EM_CLUSTERER_HPP

#include <string>
#include <vector>
#include <map>

template<typename InputFeatures, typename Estimator>
class em_clusterer
{
public:

    typedef Estimator     estimator_t;
    typedef InputFeatures input_features_t;
    typedef typename estimator_t::features_t estimator_features_t;

    typedef std::map<
        std::string,
        std::pair<double, bool>
    > sample_cluster_state_t;

    em_clusterer()
    { }

    void add_cluster(
        const std::string & cluster_uid,
        typename Estimator::ptr_t estimator);

    void drop_cluster(
        const std::string & cluster_uid);

    void add_sample(
        const std::string & uid,
        const typename input_features_t::ptr_t & feat,
        // {'cluster-uid': (P(c|s), is-hard)}
        const sample_cluster_state_t & cluster_probs
    );

    void drop_sample(const std::string & uid);

    sample_cluster_state_t get_sample_probabilities(
        const std::string & sample_uid);

    typename estimator_features_t::ptr_t get_estimator_features(
        const std::string & sample_uid);

    double get_sample_likelihood(const std::string & uid);

    template<typename FeatureTransform>
    unsigned transform_features(
        const typename FeatureTransform::ptr_t &);

    double expect_and_maximize();

private:

    struct sample_t {

        // P(sample | class)
        std::vector<double> prob_sample_class;

        // P(class | sample)
        std::vector<double> prob_class_sample;

        // whether P(cluster|sample) is 'hard'
        std::vector<bool> is_hard;

        typename input_features_t::ptr_t   input_features;
        typename estimator_features_t::ptr_t est_features;

        // unnormalized sum{ P(sample | class) for class in C}
        double prob_sample;

        void norm_class_probs();
    };

    typedef std::map<std::string, sample_t> samples_t;
    samples_t _samples;

    std::vector<std::string> _clusters;
    std::vector<typename Estimator::ptr_t> _estimators;
};

#include "em_clusterer.impl.hpp"

#endif
