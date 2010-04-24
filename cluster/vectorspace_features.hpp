#ifndef VECTORSPACE_FEATURES
#define VECTORSPACE_FEATURES

#include "cluster/sample.hpp"
#include "cluster/featurizer.hpp"
#include <vector>

namespace cluster
{

class sparse_vectorspace_features :
    public sample_features,
    public std::vector< std::pair<unsigned, float> >
{
public:

    typedef bind_ptr<sparse_vectorspace_features>::ptr_t ptr_t;

    void normalize()
    {
        float norm = 0;
        for(size_t i = 0; i != size(); ++i)
            norm += (*this)[i].second * (*this)[i].second;

        norm = 1.0 / std::sqrt(norm);

        for(size_t i = 0; i != size(); ++i)
            (*this)[i].second *= norm;
    }
};

class dense_vectorspace_features :
    public sample_features,
    public std::vector<float>
{
public:

    typedef bind_ptr<dense_vectorspace_features>::ptr_t ptr_t;

    void normalize()
    {
        float norm = 0;
        for(size_t i = 0; i != size(); ++i)
            norm += (*this)[i] * (*this)[i];

        norm = 1.0 / std::sqrt(norm);

        for(size_t i = 0; i != size(); ++i)
            (*this)[i] *= norm;
    }
};

};

#endif
