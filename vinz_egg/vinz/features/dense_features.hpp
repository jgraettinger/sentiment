#ifndef FEATURES_DENSE_FEATURES_HPP
#define FEATURES_DENSE_FEATURES_HPP

#include "features/traits.hpp"
#include "ref_counted.hpp"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <armadillo>

namespace features
{

class dense_features :
    public static_ref_counted<dense_features>,
    public arma::rowvec
{
public:

    // ptr to dense_features is immutable by default
    typedef bind_ptr<const dense_features>::ptr_t ptr_t;
    typedef bind_ptr<dense_features>::ptr_t mutable_ptr_t;

    dense_features(const std::vector<double> & v)
     : arma::rowvec( arma::conv_to<arma::rowvec>::from(v))
    { }

    dense_features(unsigned n_features)
     : arma::rowvec(n_features)
    { fill(0); }

    double inner_product(const dense_features & other)
    {
        return arma::dot(*this, other);
    }
};

// trait specializations for dense_features

template<>
struct iterator<dense_features>
{
    typedef boost::zip_iterator< boost::tuple<
        boost::counting_iterator<unsigned>,
        dense_features::const_iterator> > type;

    static type begin(const dense_features::ptr_t & f)
    {
        return boost::make_zip_iterator( boost::make_tuple(
            boost::make_counting_iterator((unsigned)0),
            f->begin()));
    }

    static type end(const dense_features::ptr_t & f)
    {
        return boost::make_zip_iterator( boost::make_tuple(
            boost::make_counting_iterator((unsigned)f->n_cols),
            f->end()));
    }
};

template<>
inline unsigned deref_id(const iterator<dense_features>::type & it)
{ return it->get<0>(); };

template<>
inline double deref_value(const iterator<dense_features>::type & it)
{ return it->get<1>(); }

};

#endif

