#include "cluster/estimator.hpp"
#include <boost/python.hpp>

namespace cluster
{
namespace bpl = boost::python;

void make_estimator_bindings()
{
    bpl::class_<estimator, estimator::ptr_t, boost::noncopyable>(
        "estimator", bpl::no_init)
    .add_property("featurizer", &estimator::get_featurizer)
    .def("add_sample", &estimator::add_sample)
    .def("drop_sample", &estimator::drop_sample)
    .def("reset_estimator", &estimator::reset_estimator)
    .def("add_sample_probability", &estimator::add_sample_probability)
    .def("prepare_estimator", &estimator::prepare_estimator)
    .def("estimate_sample", &estimator::estimate_sample);
}

};

