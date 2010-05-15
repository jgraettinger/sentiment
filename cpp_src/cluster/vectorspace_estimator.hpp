
#ifndef VECTORSPACE_ESTIMATOR_HPP
#define VECTORSPACE_ESTIMATOR_HPP

#include "cluster/vectorspace_features.hpp"
#include "cluster/estimator.hpp"
#include "cluster/sample.hpp"
#include "cluster/featurizer.hpp"
#include <boost/shared_ptr.hpp>

#include <iostream>

namespace cluster
{

class sparse_vectorspace_estimator
    : public estimator
{
public:

    typedef boost::shared_ptr<sparse_vectorspace_estimator> ptr_t;

    sparse_vectorspace_estimator(
        const featurizer::ptr_t & f,
        double min_cluster_prob
    )
     : estimator(f),
       _min_prob(min_cluster_prob)
    { }

    /// Estimator interface

    // Prepare for a sequence of calls to add_item_estimate
    void reset_estimator()
    { _centroid.clear(); }

    // train on sample & P(class | sample)
    void add_sample_probability(
        const sample_features::ptr_t & f, double prob_class_item)
    {
//        std::cout << prob_class_item << std::endl;
        
        if(prob_class_item < _min_prob)
            return;

        sparse_vectorspace_features & feat(
            dynamic_cast<sparse_vectorspace_features&>(*f));

        for(size_t i = 0; i != feat.size(); ++i)
            _centroid[feat[i].first] += prob_class_item * feat[i].second;
    }

    void prepare_estimator()
    {
        // Normalize to unit length
        double norm = 0;
        for(centroid_t::iterator it = _centroid.begin();
            it != _centroid.end(); ++it)
        {
            norm += it->second * it->second;
        }

        norm = 1.0 / std::sqrt(norm);

        for(centroid_t::iterator it = _centroid.begin();
            it != _centroid.end(); ++it)
        {
            it->second *= norm;
        
//            std::cout << it->first << ": " << it->second << ", ";
        }
        std::cout << std::endl;
        return;
    }

    // decode prob(sample | class)
    double estimate_sample(const sample_features::ptr_t & f)
    {
        sparse_vectorspace_features & feat(
            dynamic_cast<sparse_vectorspace_features&>(*f));

        double p_item = 0;
        for(size_t i = 0; i != feat.size(); ++i)
        {
            centroid_t::const_iterator it(
                _centroid.find(feat[i].first));

            if(it == _centroid.end())
                continue;

            p_item += it->second * feat[i].second;
        }
        
        return p_item * p_item * p_item;
    }

private:

    typedef boost::unordered_map<unsigned, double> centroid_t;

    double _min_prob;
    centroid_t _centroid;
};


class dense_vectorspace_estimator
    : public estimator
{
public:

    typedef boost::shared_ptr<dense_vectorspace_estimator> ptr_t;

    dense_vectorspace_estimator(
        const featurizer::ptr_t & f,
        double min_cluster_prob
    )
     : estimator(f),
       _min_prob(min_cluster_prob)
    { }

    /// Estimator interface

    // Prepare for a sequence of calls to add_item_estimate
    void reset_estimator()
    { _centroid.clear(); }

    // train on sample & P(class | sample)
    void add_sample_probability(
        const sample_features::ptr_t & f, double prob_class_item)
    {
        if(prob_class_item < _min_prob)
            return;

        dense_vectorspace_features & feat(
            dynamic_cast<dense_vectorspace_features&>(*f));

        if(!_centroid.size())
            _centroid.resize(feat.size(), 0);

        if(_centroid.size() != feat.size())
            throw std::runtime_error("feature arity mismatch");

        for(size_t i = 0; i != feat.size(); ++i)
            _centroid[i] += prob_class_item * feat[i];
    }

    void prepare_estimator()
    {
        // Normalize to unit length
        double norm = 0;
        for(centroid_t::iterator it = _centroid.begin();
            it != _centroid.end(); ++it)
        {
            norm += *it * *it;
        }

        norm = 1.0 / std::sqrt(norm);

        for(centroid_t::iterator it = _centroid.begin();
            it != _centroid.end(); ++it)
        {
            *it *= norm;
        }
        return;
    }

    // decode prob(sample | class)
    double estimate_sample(const sample_features::ptr_t & f)
    {
        dense_vectorspace_features & feat(
            dynamic_cast<dense_vectorspace_features&>(*f));

        if(_centroid.size() != feat.size())
            throw std::runtime_error("feature arity mismatch");

        double p_item = 0;
        for(size_t i = 0; i != feat.size(); ++i)
        {
            p_item += _centroid[i] * feat[i];
        }
        return p_item;
    }

private:

    typedef std::vector<double> centroid_t;

    double _min_prob;
    centroid_t _centroid;
};

};

#endif
