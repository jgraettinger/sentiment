#if 0

#include "estimation/naive_bayes_estimator.hpp"
#include <boost/python.hpp>

namespace estimation {
namespace bpl = boost::python;

void make_naive_bayes_estimator_bindings()
{
    bpl::class_<naive_bayes_estimator,
        naive_bayes_estimator::ptr_t, boost::noncopyable>(
        "NaiveBayesEstimator", bpl::init<double>(bpl::args("alpha")))
        .def("reset", &naive_bayes_estimator::reset)
        .def("add_observation", &naive_bayes_estimator::add_observation)
        .def("prepare_estimator", &naive_bayes_estimator::prepare_estimator)
        .def("estimate", &naive_bayes_estimator::estimate);
}

};
#endif
