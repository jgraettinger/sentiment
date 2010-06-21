
#include "cluster/em_clusterer.hpp"
#include "cluster/naive_bayes_estimator.hpp"
#include <boost/python.hpp>

namespace cluster
{
namespace bpl = boost::python;

template<
    typename Estimator,
    typename FeatureSelector
>
void bind_em_clusterer(const char * name)
{
    typedef em_clusterer<
        Estimator,
        FeatureSelector
    > em_clusterer_t;

    bpl::class_<em_clusterer_t>(name, bpl::init<>())
    .def("add_cluster", &em_clusterer_t::add_cluster)
    .def("drop_cluster", &em_clusterer_t::drop_cluster)
    .def("add_sample", &em_clusterer_t::add_sample)
    .def("drop_sample", &em_clusterer_t::drop_sample)
    .def("get_sample_probabilities", &em_clusterer_t::get_sample_probabilities)
    .def("iterate", &em_clusterer_t::iterate);
}

void make_em_clusterer_bindings()
{
    bind_em_clusterer<naive_bayes_estimator, void>(
        "bayes_em_clusterer");
}

};