
#ifndef CLUSTER_FEATURE_SELECTION_FILTER_MIXIN_HPP
#define CLUSTER_FEATURE_SELECTION_FILTER_MIXIN_HPP

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

    typedef std::vector<std::pair<unsigned, double> > feature_stat_t;
    typedef boost::unordered_set<unsigned> feature_index_t;

    unsigned prepare_selector()
    {
        _index.clear();

        feature_stat_t features = \
            static_cast<Derived*>(this)->select_features();

        for(typename feature_stat_t::iterator it = features.begin();
            it != features.end(); ++it)
        {
            _index.insert( it->first);
        }
        return features.size();
    }

    typename Features::mutable_ptr_t filter_features(
        const typename Features::ptr_t & fptr)
    {
        const Features & feat(*fptr);
        typename Features::mutable_ptr_t new_fptr(new Features());

        Features & filt_feat(*new_fptr);
        filt_feat.reserve(feat.size());

        for(unsigned i = 0; i != feat.size(); ++i)
        {
            // is this a selected feature?
            if(_index.find(feat[i].first) != _index.end())
            {
                filt_feat.push_back(feat[i]);
            }
        }
        return new_fptr;
    }

private:

    feature_index_t _index;
};

};
};

#endif

