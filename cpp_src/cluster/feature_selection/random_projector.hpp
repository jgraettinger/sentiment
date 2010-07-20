#ifndef CLUSTER_FEATURE_SELECTOR_RANDOM_PROJECTOR_HPP
#define CLUSTER_FEATURE_SELECTOR_RANDOM_PROJECTOR_HPP

#include <boost/functional/hash.hpp>
#include <boost/unordered_set.hpp>
#include <vector>

namespace cluster
{
namespace feature_selection
{

template<typename Derived, typename InputFeatures, typename OutputFeatures>
class random_projector_mixin
{
public:

    typedef std::vector<std::pair<unsigned, double> > feature_stat_t;
    typedef boost::unordered_set<unsigned> feature_index_t;

    random_projector_mixin(unsigned n_output_features)
     : _root_3(std::sqrt(3.0)),
       _n_output_features(n_output_features) 
    { }

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

    typename OutputFeatures::mutable_ptr_t filter_features(
        const typename InputFeatures::ptr_t & if_ptr)
    {
        typename OutputFeatures::mutable_ptr_t of_ptr(
            new OutputFeatures(_n_output_features));

        const InputFeatures & ifeat(*if_ptr);
        OutputFeatures & ofeat(*of_ptr);

        for(typename InputFeatures::const_iterator it = ifeat.begin();
            it != ifeat.end(); ++it)
        {
            if(_index.find(it->first) == _index.end())
                continue;

            for(unsigned i = 0; i != _n_output_features; ++i)
            {
                size_t h = 0;

                boost::hash_combine(h, it->first);
                boost::hash_combine(h, i);

                h = h % 6;

                if(h == 0)
                    ofeat[i] += _root_3 * it->second;
                if(h == 1)
                    ofeat[i] -= _root_3 * it->second;
            }
        }

        return of_ptr;
    }

private:

    const double _root_3;
    const unsigned _n_output_features;

    feature_index_t _index;
};

}

}
#endif
