#ifndef FEATURE_TRANSFORM_TRANSFORM_HELPER_HPP
#define FEATURE_TRANSFORM_TRANSFORM_HELPER_HPP

#include "features/traits.hpp"
#include "util.hpp"
#include <boost/iterator/zip_iterator.hpp>
#include <boost/python.hpp>
#include <vector>

namespace feature_transform
{
namespace bpl = boost::python;

template<typename Transform, typename Features>
void py_train_transform_helper(
    typename Transform::ptr_t transform,
    const std::vector< typename features::value<Features>::type > & vec_feat,
    const std::vector< std::vector<double> > & vec_prob)
{
    if(vec_feat.size() != vec_prob.size())
        throw std::runtime_error("feature & probability sequence arity mismatch");

    if(vec_feat.empty())
        throw std::runtime_error("called with empty sequence");

    typename Transform::template train_transform<
        Features> train_transform(transform);

    train_transform(
        boost::make_zip_iterator( boost::make_tuple(
            vec_feat.begin(), vec_prob.begin())),
        boost::make_zip_iterator( boost::make_tuple(
            vec_feat.end(),   vec_prob.end())));
}

template<typename Transform, typename Features>
typename features::value<
    typename Transform::template transform<Features>::output_features_t>::type
py_transform_helper(
    typename Transform::ptr_t transform,
    typename features::reference<Features>::type feat)
{
    return typename Transform::template transform<Features>(transform)(feat);
}

};

#endif

