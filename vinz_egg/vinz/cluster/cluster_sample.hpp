#ifndef VINZ_CLUSTER_CLUSTER_SAMPLE_HPP
#define VINZ_CLUSTER_CLUSTER_SAMPLE_HPP

#include "cluster/cluster_set.hpp"
#include "ref_counted.hpp"
#include <boost/python.hpp>
#include <vector>

namespace cluster
{

template<typename InputFeatures, typename EstimatorFeatures>
struct cluster_sample :
    public static_ref_counted<cluster_sample<InputFeatures, EstimatorFeatures> >
{
public:

    typedef InputFeatures     input_features_t;
    typedef EstimatorFeatures estimator_features_t;

    typedef typename static_ref_counted<cluster_sample>::\
        template bind_ptr<cluster_sample>::ptr_t ptr_t;

    cluster_sample(
        const std::string & uid,
        const typename input_features_t::ptr_t & input_features,
        const cluster_set::ptr_t & cluster_set);

    void notify_of_added_cluster(size_t index);
    void notify_of_dropped_cluster(size_t index);

    boost::python::object get_cluster_probabilities();
    void set_cluster_probabilities(boost::python::object);

    const std::string uid;

    // P(sample | cluster)
    std::vector<double> prob_sample_cluster;

    // P(cluster | sample)
    std::vector<double> prob_cluster_sample;

    // whether P(cluster|sample) is fixed in place, or
    //  may be adjusted during the expectation step
    std::vector<bool> is_hard;

    // relative weight of sample, as compared to peers
    double weight;

    typename input_features_t::ptr_t   input_features;
    typename estimator_features_t::ptr_t est_features;

    // P(sample) = sum{P(sample | cluster) for cluster in C}
    double prob_sample;

    void norm_cluster_probs();

private:

    cluster_set::ptr_t _cluster_set;
};

#include "cluster_sample.impl.hpp"

};

#endif
