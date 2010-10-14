#include "cluster/cluster_sample.hpp"
#include "features/dense_features.hpp"
#include "features/sparse_features.hpp"
#include <boost/python.hpp>

namespace bpl = boost::python;

namespace cluster
{

using namespace features;

template<typename ClusterSample>
void bind_cluster_sample(const char * name)
{
    bpl::class_<ClusterSample, typename ClusterSample::ptr_t,
        boost::noncopyable>(name, bpl::no_init)
    .add_property("cluster_probabilities",
        &ClusterSample::get_cluster_probabilities,
        &ClusterSample::set_cluster_probabilities)
    .def_readonly("uid", &ClusterSample::uid)
    .def_readwrite("weight", &ClusterSample::weight)
    .def_readonly("input_features", &ClusterSample::input_features)
    .add_property("estimator_features", bpl::make_getter(
        &ClusterSample::est_features,
        bpl::return_value_policy<bpl::return_by_value>()))
    .def_readonly("probability", &ClusterSample::prob_sample);
}

void make_cluster_sample_bindings()
{
    bind_cluster_sample<cluster_sample<dense_features, dense_features> >(
        "cluster_sample_dense_dense");
    bind_cluster_sample<cluster_sample<sparse_features, dense_features> >(
        "cluster_sample_sparse_dense");
}

};

