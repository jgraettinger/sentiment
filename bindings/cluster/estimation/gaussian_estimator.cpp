#include "cluster/estimation/gaussian_estimator.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace estimation {
namespace bpl = boost::python;

void make_gaussian_estimator_bindings()
{
    bpl::class_<gaussian_estimator,
        gaussian_estimator::ptr_t, boost::noncopyable>(
        "GaussianEstimator", bpl::init<unsigned>(bpl::args("n_features")))
        .def("reset", &gaussian_estimator::reset)
        .def("add_observation", &gaussian_estimator::add_observation)
        .def("prepare_estimator", &gaussian_estimator::prepare_estimator)
        .def("estimate", &gaussian_estimator::estimate);
}

};
};

