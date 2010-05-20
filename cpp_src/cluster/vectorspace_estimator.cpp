#include "cluster/vectorspace_estimator.hpp"
#include "cluster/vectorspace_features.hpp"
#include "cluster/featurizer.hpp"
#include "util.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace bpl = boost::python;

void make_vectorspace_estimator_bindings()
{
    bpl::class_<sparse_vectorspace_estimator,
        sparse_vectorspace_estimator::ptr_t,
        bpl::bases<estimator> >(
        "sparse_vectorspace_estimator",
        bpl::init<const featurizer::ptr_t &, double>());

    bpl::class_<dense_vectorspace_estimator,
        dense_vectorspace_estimator::ptr_t,
        bpl::bases<estimator> >(
        "dense_vectorspace_estimator",
        bpl::init<const featurizer::ptr_t &, double>());

    bpl::class_<gaussian_estimator,
        gaussian_estimator::ptr_t,
        bpl::bases<estimator> >(
        "gaussian_estimator",
        bpl::init<const featurizer::ptr_t &>());

    bpl::class_<unigram_lm_estimator,
        unigram_lm_estimator::ptr_t,
        bpl::bases<estimator> >(
        "unigram_lm_estimator",
        bpl::init<const featurizer::ptr_t &>());
}

};
