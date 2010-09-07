#ifndef FEATURES_SPARSE_FEATURES_HPP
#define FEATURES_SPARSE_FEATURES_HPP

#include "ref_counted.hpp"
#include <algorithm>
#include <vector>
#include <map>

namespace features
{

class sparse_features :
    public static_ref_counted<sparse_features>,
    public std::vector< std::pair<unsigned, double> >
{
public:
    
    typedef unsigned key_type;
    typedef double value_type;

    // ptr to sparse_features is immutable by default
    typedef bind_ptr<const sparse_features>::ptr_t ptr_t;
    typedef bind_ptr<sparse_features>::ptr_t mutable_ptr_t;

    sparse_features()
    { }

    sparse_features(unsigned n_features)
    { reserve(n_features); }

    sparse_features(const std::map<unsigned, double> & m)
    {
        resize(m.size());
        std::copy(m.begin(), m.end(), begin());
        std::sort(begin(), end());
        return;
    }

    double inner_product(const sparse_features & other)
    {
        const_iterator  it1 = begin(), it2 = other.begin();
        const_iterator end1 = end(),  end2 = other.end();

        double prod = 0;
        while(it1 != end1 && it2 != end2)
        {
            if(it1->first < it2->first)
                ++it1;
            else if(it1->first > it2->first)
                ++it2;
            else
            {
                prod += it1->second * it2->second;
                ++it1; ++it2;
            }
        }
        return prod;
    }
};

};

#endif
