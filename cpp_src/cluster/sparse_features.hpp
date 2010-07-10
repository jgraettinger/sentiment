#ifndef CLUSTER_SPARSE_FEATURES_HPP
#define CLUSTER_SPARSE_FEATURES_HPP

#include "cluster/ref_counted.hpp"
#include <algorithm>
#include <vector>
#include <map>

namespace cluster
{

class sparse_features :
    public static_ref_counted<sparse_features>,
    public std::vector< std::pair<unsigned, double> >
{
public:

    // ptr to sparse_features is immutable by default
    typedef bind_ptr<const sparse_features>::ptr_t ptr_t;
    typedef bind_ptr<sparse_features>::ptr_t mutable_ptr_t;

    sparse_features()
    { }

    sparse_features(const std::map<unsigned, double> & m)
    {
        resize(m.size());
        std::copy(m.begin(), m.end(), begin());
        std::sort(begin(), end());
        return;
    }
};

};

#endif
