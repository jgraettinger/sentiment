
#ifndef CLUSTER_FEATURE_SELECTION_FILTER_MIXIN_HPP
#define CLUSTER_FEATURE_SELECTION_FILTER_MIXIN_HPP

#include "cluster/vector_ops.hpp"
#include <boost/unordered_set.hpp>
#include <vector>

namespace cluster
{
namespace feature_selection
{

template<typename Derived, typename Features>
class filter_mixin
{
public:

    typedef std::vector< std::pair<unsigned, double> > feature_stat_t;
    typedef boost::unordered_set<unsigned> feature_index_t;

    unsigned prepare_selector()
    {
        _index.clear();

        feature_stat_t features = \
            static_cast<Derived*>(this)->select_features();

        for(feature_stat_t::iterator it = features.begin();
            it != features.end(); ++it)
        {
            _index.insert( it->first);
        }
        return features.size();
    }

    typename Features::mutable_ptr_t filter_features(const Features & feat)
    {
        typename Features::mutable_ptr_t fptr(new Features());

        Features & filt_feat(*fptr);
        filt_feat.reserve(feat.size());

        for(unsigned i = 0; i != feat.size(); ++i)
        {
            // is this a selected feature?
            if(_index.find(feat[i].first) != _index.end())
            {
                filt_feat.push_back(feat[i]);
            }
        }
        return fptr;
    }

    typename Features::mutable_ptr_t filter_and_normalize_features(
        const Features & feat)
    {
        typename Features::mutable_ptr_t fptr( filter_features(feat));

        vector_ops::normalize_L1( *fptr);
        return fptr;
    }

private:

    feature_index_t _index;
};

};
};

#endif

