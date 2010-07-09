#ifndef CLUSTER_DENSE_FEATURES_HPP
#define CLUSTER_DENSE_FEATURES_HPP

#include "cluster/ref_counted.hpp"
#include <armadillo>

namespace cluster
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
};

};

#endif

