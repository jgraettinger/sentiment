
#include "cluster/em_clusterer.hpp"
#include "cluster/sparse_features.hpp"
#include "cluster/dense_features.hpp"
#include "cluster/estimation/naive_bayes_estimator.hpp"
#include "cluster/estimation/gaussian_estimator.hpp"
#include "cluster/feature_selection/information_gain_selector.hpp"
#include <boost/python.hpp>

namespace cluster
{
namespace bpl = boost::python;

template<typename Clusterer>
bpl::class_<Clusterer> bind_em_clusterer(const char * name)
{
    return bpl::class_<Clusterer>(name, bpl::init<>())
    .def("add_cluster", &Clusterer::add_cluster)
    .def("drop_cluster", &Clusterer::drop_cluster)
    .def("add_sample", &Clusterer::add_sample)
    .def("drop_sample", &Clusterer::drop_sample)
    .def("get_sample_probabilities", &Clusterer::get_sample_probabilities)
    .def("get_estimator_features", &Clusterer::get_estimator_features)
    .def("expect_and_maximize", &Clusterer::expect_and_maximize)
    .def("get_sample_likelihood", &Clusterer::get_sample_likelihood);
}

void make_em_clusterer_bindings()
{
/*    bind_em_clusterer<
        estimation::naive_bayes_estimator,
        feature_selection::information_gain_selector
    >("NaiveBayesEmClusterer");
*/
    bind_em_clusterer< em_clusterer<
        dense_features,
        estimation::gaussian_estimator>
    >("DenseGaussEmClusterer");
}

};
