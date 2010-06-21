
#ifndef VECTORSPACE_ESTIMATOR_HPP
#define VECTORSPACE_ESTIMATOR_HPP

#include "cluster/vectorspace_features.hpp"
#include "cluster/estimator.hpp"
#include "cluster/sample.hpp"
#include "cluster/featurizer.hpp"
#include <boost/shared_ptr.hpp>

#include <iostream>

#define __MATH_E 2.71828182845904523536
#define __MATH_PI 3.1415926535897932384626433832795028

namespace cluster
{

class unigram_lm_estimator
    : public estimator
{
public:

    typedef boost::shared_ptr<unigram_lm_estimator> ptr_t;

    unigram_lm_estimator(const featurizer::ptr_t & f)
     : estimator(f),
       _prob_unk(1),
       _alpha(0.04)
    { }

    /// Estimator interface

    // Prepare for a sequence of calls to add_item_estimate
    void reset_estimator()
    {
        _est.clear();
        _prob_unk = 1;
    }

    // train on sample & P(class | sample)
    void add_sample_probability(
        const sample_features::ptr_t & f, double prob_class_item)
    {
        if(prob_class_item <= 0)
            return;

        sparse_vectorspace_features & feat(
            dynamic_cast<sparse_vectorspace_features&>(*f));

        for(size_t i = 0; i != feat.size(); ++i)
        {
            est_t::iterator it = _est.find(feat[i].first);
            double alpha = _alpha * prob_class_item;

            if(it == _est.end())
            {
                _est[feat[i].first] = prob_class_item * feat[i].second;

                _prob_unk = alpha * 1 + (1 - alpha) * _prob_unk;
            }
            else
            {
                it->second += prob_class_item * feat[i].second;

                _prob_unk = alpha * 0 + (1 - alpha) * _prob_unk;
            }
        }

        return;
    }

    void prepare_estimator()
    {
//        std::cout << "P(<unk>) == " << _prob_unk << std::endl;

        // add <unk>
        double mass = _prob_unk;

        for(est_t::iterator it = _est.begin();
            it != _est.end(); ++it)
        {
            mass += it->second;
        }

        double norm = 1.0 / mass;

        for(est_t::iterator it = _est.begin();
            it != _est.end(); ++it)
        {
            it->second = std::log(it->second * norm);
        }

        _prob_unk = std::log(_prob_unk * norm);
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
            est_t::const_iterator it(
                _est.find(feat[i].first));

            if(it == _est.end())
            {
                p_item += (1.0 * _prob_unk);
                continue;
            }

            p_item += (1.0 * it->second) * feat[i].second;
        }
        return p_item;
    }

private:

    typedef boost::unordered_map<unsigned, double> est_t;

    double _prob_unk;
    double _alpha;
    est_t _est;
};

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
        if(prob_class_item < _min_prob)
            return;

        sparse_vectorspace_features & feat(
            dynamic_cast<sparse_vectorspace_features&>(*f));

        for(size_t i = 0; i != feat.size(); ++i)
            _centroid[feat[i].first] += prob_class_item * feat[i].second;

        return;
    }

    void prepare_estimator()
    {
        double norm = 0;
        for(centroid_t::iterator it = _centroid.begin();
            it != _centroid.end(); ++it)
        {
            // amplify signal in the model
            it->second = std::pow(it->second, 1.1);
            norm += it->second;
        }

        norm = 1.0 / norm;

//        std::cout << "mean: ";
        for(centroid_t::iterator it = _centroid.begin();
            it != _centroid.end(); ++it)
        {
            it->second *= norm;

//            std::cout << it->first << ": " << it->second << ", ";
        }
//        std::cout << std::endl;
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
        return p_item;
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
       _norm(0),
       _min_prob(min_cluster_prob)
    { }

    /// Estimator interface

    // Prepare for a sequence of calls to add_item_estimate
    void reset_estimator()
    { _centroid.clear(); _norm = 0; }

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

        _norm += prob_class_item;
        return;
    }

    void prepare_estimator()
    { }

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
        return p_item / _norm;
    }

private:

    typedef std::vector<double> centroid_t;

    double _norm;
    double _min_prob;
    centroid_t _centroid;
};

class gaussian_estimator
    : public estimator
{
public:

    typedef boost::shared_ptr<gaussian_estimator> ptr_t;

    gaussian_estimator(const featurizer::ptr_t & f)
     : estimator(f), _norm(0)
    { }

    void reset_estimator()
    {
        _mean.clear();
        _stdev.clear();
        _feats.clear();
        _norm = 0;
    }

    void add_sample_probability(
        const sample_features::ptr_t & f, double prob_class_item)
    {
        dense_vectorspace_features & feat(
            dynamic_cast<dense_vectorspace_features&>(*f));

        if(_mean.empty())
        {
            _mean.resize(feat.size());
            _stdev.resize(feat.size());
        }
        else if(_mean.size() != feat.size())
            throw std::runtime_error("feature arity mismatch");

        for(size_t i = 0; i != feat.size(); ++i)
        {
            _mean[i] += feat[i] * prob_class_item;
        }
        _norm += prob_class_item;
        _feats.push_back(std::make_pair(f, prob_class_item));
        return;
    }

    void prepare_estimator()
    {
        _norm = 1.0 / _norm;

        for(unsigned i = 0; i != _mean.size(); ++i)
            _mean[i] *= _norm;

        for(unsigned f = 0; f != _feats.size(); ++f)
        {
            dense_vectorspace_features & feat(
                dynamic_cast<dense_vectorspace_features&>(*_feats[f].first));

            for(unsigned i = 0; i != feat.size(); ++i)
            {
                double t = feat[i] - _mean[i]; 
                _stdev[i] += t * t * _feats[f].second * _norm;
            }
        }

        for(unsigned i = 0; i != _stdev.size(); ++i)
            _stdev[i] = std::sqrt(_stdev[i]);

        return;
    }

    // decode prob(sample | class)
    double estimate_sample(const sample_features::ptr_t & f)
    {
        dense_vectorspace_features & feat(
            dynamic_cast<dense_vectorspace_features&>(*f));

        if(feat.size() != _mean.size())
            throw std::runtime_error("feature arity mismatch");

        double p_item = 1.0;
        for(unsigned i = 0; i != _mean.size(); ++i)
        {
            double t = feat[i] - _mean[i];
            double var = _stdev[i] * _stdev[i];
            t = std::pow(__MATH_E, -1 * t * t / (2 * var));
            t /= std::sqrt(2 * __MATH_PI * var);

            p_item *= t;
        }
        return std::log(p_item);
    }

private:

    std::vector<std::pair<sample_features::ptr_t, double> > _feats;

    double _norm;
    std::vector<double> _mean;
    std::vector<double> _stdev;
};
};

#endif
