#include "estimation/inner_product_estimator.hpp"
#include <boost/python.hpp>

namespace estimation {
namespace bpl = boost::python;

void make_inner_product_estimator_bindings()
{
    bpl::class_<inner_product_estimator,
        inner_product_estimator::ptr_t, boost::noncopyable>(
        "InnerProductEstimator", bpl::init<>())
        .def("reset", &inner_product_estimator::reset)
        .def("add_observation", &inner_product_estimator::add_observation)
        .def("prepare_estimator", &inner_product_estimator::prepare_estimator)
        .def("estimate", &inner_product_estimator::estimate)
        .def("distance", &inner_product_estimator::distance)
        .def("inner_product", &inner_product_estimator::inner_product)
        .def("get_mean", &inner_product_estimator::get_mean)
        .def("get_std_deviation", &inner_product_estimator::get_std_deviation)
        .add_property("soft_prob_coeff",
            &inner_product_estimator::get_soft_prob_coeff,
            &inner_product_estimator::set_soft_prob_coeff);
}

};

