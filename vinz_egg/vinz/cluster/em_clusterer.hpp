
#ifndef CLUSTER_EM_CLUSTERER_HPP
#define CLUSTER_EM_CLUSTERER_HPP

#include "cluster_set.hpp"
#include "cluster_sample.hpp"
#include <boost/python.hpp>
#include <string>
#include <vector>
#include <map>

namespace cluster
{

template<typename InputFeatures, typename Estimator>
class em_clusterer
{
public:

    typedef Estimator     estimator_t;
    typedef InputFeatures input_features_t;
    typedef typename estimator_t::features_t estimator_features_t;
    typedef cluster_sample<
        input_features_t, estimator_features_t> cluster_sample_t;

    typedef std::map<
        std::string,
        std::pair<double, bool>
    > sample_cluster_state_t;

    em_clusterer();

    void add_cluster(
        const std::string & cluster_uid,
        typename Estimator::ptr_t estimator);

    void drop_cluster(
        const std::string & cluster_uid);

    cluster_set::ptr_t get_cluster_set()
    { return _cluster_set; }

    typename cluster_sample_t::ptr_t add_sample(
        const std::string & uid,
        const typename input_features_t::ptr_t & input_features
    );

    typename cluster_sample_t::ptr_t get_sample(const std::string & uid);

    boost::python::list py_samples();

    void drop_sample(const std::string & uid);

    template<typename FeatureTransform>
    void transform_features(const typename FeatureTransform::ptr_t &);

    double expect_and_maximize();

private:

    typedef std::map<std::string, typename cluster_sample_t::ptr_t> samples_t;
    samples_t _samples;

    cluster_set::ptr_t _cluster_set;
    std::vector<typename Estimator::ptr_t> _estimators;
};

};

#include "em_clusterer.impl.hpp"

#endif
