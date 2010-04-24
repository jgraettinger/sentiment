#ifndef FEATURIZER_HPP
#define FEATURIZER_HPP

#include "cluster/sample.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace cluster {

class featurizer
{
public:

    typedef boost::shared_ptr<featurizer> ptr_t;

    virtual sample_features::ptr_t featurize(const sample::ptr_t &) = 0;
};

class cached_featurizer : public featurizer
{
public:

    typedef boost::shared_ptr<cached_featurizer> ptr_t;

    cached_featurizer(const featurizer::ptr_t & f)
     : _f(f)
    { }

    sample_features::ptr_t featurize(const sample::ptr_t & s)
    {
        cache_t::iterator it( _cache.find(s->get_uid()));

        if(it != _cache.end())
            return it->second;

        sample_features::ptr_t feat( _f->featurize(s));
        _cache[s->get_uid()] = feat;
        return feat;
    }

    void drop(const sample::ptr_t & s)
    {
        cache_t::iterator it( _cache.find(s->get_uid()));

        if(it != _cache.end())
            _cache.erase(it);
    }

    featurizer::ptr_t get_wrapped_featurizer()
    { return _f; }

private:

    typedef boost::unordered_map<
        std::string, sample_features::ptr_t> cache_t;
 
    // delegated featurizer
    featurizer::ptr_t _f;

    cache_t _cache;
};

};

#endif
