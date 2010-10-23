#include "estimation/gaussian_estimator.hpp"
#include <boost/python.hpp>

namespace estimation {
namespace bpl = boost::python;

void make_gaussian_estimator_bindings()
{
    bpl::class_<gaussian_estimator,
        gaussian_estimator::ptr_t, boost::noncopyable>(
        "GaussianEstimator", bpl::init<>())
        .def("reset", &gaussian_estimator::reset)
        .def("add_observation", &gaussian_estimator::add_observation)
        .def("prepare_estimator", &gaussian_estimator::prepare_estimator)
        .def("estimate", &gaussian_estimator::estimate)
        .def("get_mean", &gaussian_estimator::get_mean)
        .def("get_covar", &gaussian_estimator::get_covar)
        .def("get_eigenvector", &gaussian_estimator::get_eigenvector)
        .def("get_eigenvalue", &gaussian_estimator::get_eigenvalue)
        .add_property("soft_prob_coeff",
            &gaussian_estimator::get_soft_prob_coeff,
            &gaussian_estimator::set_soft_prob_coeff);
}

};

