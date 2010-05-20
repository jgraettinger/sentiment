#ifndef VECTORSPACE_FEATURES
#define VECTORSPACE_FEATURES

#include "cluster/sample.hpp"
#include "cluster/featurizer.hpp"
#include <vector>

namespace cluster
{

class sparse_vectorspace_features :
    public sample_features,
    public std::vector< std::pair<unsigned, double> >
{
public:

    typedef bind_ptr<sparse_vectorspace_features>::ptr_t ptr_t;
};

class dense_vectorspace_features :
    public sample_features,
    public std::vector<double>
{
public:

    typedef bind_ptr<dense_vectorspace_features>::ptr_t ptr_t;
};

};

#endif
