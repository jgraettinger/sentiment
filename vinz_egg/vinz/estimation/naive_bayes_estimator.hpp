#error This is broken code, don't include

#ifndef NAIVE_BAYES_ESTIMATOR_HPP
#define NAIVE_BAYES_ESTIMATOR_HPP

#include "features/sparse_features.hpp"
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <cfloat>

namespace estimation
{

class naive_bayes_estimator
{
public:

    typedef boost::shared_ptr<naive_bayes_estimator> ptr_t;

    typedef features::sparse_features features_t;

    naive_bayes_estimator(double alpha)
     : _prob_unk(1),
       _alpha(alpha)
    { }

    void reset()
    {
        _est.clear();
        _prob_unk = 1;
    }

    // train on sample & P(class | sample)
    void add_observation(
        const features_t::ptr_t & fptr, double prob_class_item)
    {
        if(prob_class_item <= 0)
            return;

        const features_t & feat(*fptr);

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
        // add <unk>
        double pnorm = _prob_unk;

        for(est_t::iterator it = _est.begin();
            it != _est.end(); ++it)
        {
            pnorm += it->second;
        }

        pnorm = 1.0 / pnorm;

        for(est_t::iterator it = _est.begin();
            it != _est.end(); ++it)
        {
            it->second = std::log(it->second * pnorm);
        }

        _prob_unk = std::log(_prob_unk * pnorm);
        return;
    }

    // decode prob(sample | class)
    double estimate(const features_t::ptr_t & fptr)
    {
        if(fptr->empty())
            return -DBL_MAX;

        const features_t & feat(*fptr);

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

};

#endif
