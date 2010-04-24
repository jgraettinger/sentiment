
#ifndef SPARSE_VECTORSPACE_ESTIMATOR_HPP
#define SPARSE_VECTORSPACE_ESTIMATOR_HPP

#include "cluster/base_estimator.hpp"
#include <boost/unordered_map.hpp>

namespace cluster
{

class sparse_vectorspace_estimator : public base_estimator
{
public:

    class features :
        public base_estimator::item_features,
        public std::vector< std::pair<unsigned, float> >
    { };

    // Item interface

    features::ptr_t add_item(const std::string & item_uid);

    void drop_item(const std::string & item_uid);

    /// Estimator interface

    // Prepare for a sequence of calls to add_item_estimate
    void reset_estimator();

    // train
    void add_item_probability(
        const item_features::ptr_t, float prob_class_item);

    void prepare_estimator();

    // decode prob(item | class)
    float estimate_item(const item_features::ptr_t);

private:

    typedef boost::unordered_map<unsigned, float> estimator_t;

    estimator_t _estimator;
};


inline sparse_vectorspace_estimator::reset_estimator()
{ _estimator.clear(); }

inline sparse_vectorspace_estimator::add_item_probability(
    const base_estimator::item_features::ptr_t & f, float prob_class_item)
{
    if(prob_class_item < 0.1)
        return;

    features & feat( dynamic_cast<features&>(*f));

    for(size_t i = 0; i != feat.size(); ++i)
        _estimator[feat[i].first] += prob_class_item * feat[i].second;

    return;
}

inline sparse_vectorspace_estimator::prepare_estimator()
{
    // Normalize to unit length
    float norm = 0;
    for(estimator_t::iterator it = _estimator.begin();
        it != _estimator.end(); ++it)
    {
        norm += it->second * it->second;
    }

    norm = 1.0 / std::sqrt(norm);

    for(estimator_t::iterator it = _estimator.begin();
        it != _estimator.end(); ++it)
    {
        it->second *= norm;
    }

    return;
}

inline float sparse_vectorspace_estimator::estimate_item(
    const base_estimator::item_features::ptr_t & f)
{
    features & feat( dynamic_cast<features&>(*f));

    float p_item = 0;
    for(size_t i = 0; i != feat.size(); ++i)
    {
        estimator_t::const_iterator it(
            _estimator.find(feat[i].first));

        if(it == _estimator.end())
            continue;

        p_item += it->second * feat[i].second;
    }
    return p_item;
}

};

#endif
