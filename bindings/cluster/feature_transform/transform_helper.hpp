#ifndef BINDINGS_CLUSTER_FEATURE_TRANSFORM_TRANSFORM_HELPER_HPP
#define BINDINGS_CLUSTER_FEATURE_TRANSFORM_TRANSFORM_HELPER_HPP

#include "cluster/features/traits.hpp"
#include "util.hpp"
#include <boost/iterator/zip_iterator.hpp>
#include <boost/python.hpp>
#include <vector>

namespace cluster
{
namespace feature_transform
{
namespace bpl = boost::python;

template<typename Transform, typename Features>
void py_train_transform_helper(
    typename Transform::ptr_t transform, boost::python::list l)
{
    typedef std::vector<typename features::value<Features>::type> vec_feat_t;
    typedef std::vector<std::vector<double> > vec_prob_t;

    vec_feat_t vec_feat;
    vec_prob_t vec_prob;

    if(PyObject_HasAttrString(l.ptr(), "__len__"))
    {
        vec_feat.reserve(bpl::len(l));
        vec_prob.reserve(bpl::len(l));
    }

    for(bpl::object obj, iter = get_iterator(l); next(iter, obj);)
    {
        vec_feat.push_back( bpl::extract<
            typename vec_feat_t::value_type>(obj[0])());
        vec_prob.push_back( bpl::extract<
            typename vec_prob_t::value_type>(obj[1])());
    }

    typename Transform::template train_transform<
        Features> train_transform(transform);

    train_transform(
        boost::make_zip_iterator( boost::make_tuple(
            vec_feat.begin(), vec_prob.begin())),
        boost::make_zip_iterator( boost::make_tuple(
            vec_feat.end(),   vec_prob.end())));
};

template<typename Transform, typename Features>
typename features::value<
    typename Transform::template transform<Features>::result_type>::type
py_transform_helper(
    typename Transform::ptr_t transform,
    typename features::reference<Features>::type feat)
{
    return typename Transform::template transform<Features>(transform)(feat);
};


template<typename Transform, typename Features>
void make_transform_bindings(std::string transform_name)
{


}

};
};

#endif

