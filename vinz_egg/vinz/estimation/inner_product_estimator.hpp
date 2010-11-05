#ifndef ESTIMATION_INNER_PRODUCT_ESTIMATOR_HPP
#define ESTIMATION_INNER_PRODUCT_ESTIMATOR_HPP

#include "features/sparse_features.hpp"
#include "features/traits.hpp"
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include <vector>
#include <cmath>
#include <limits>

namespace estimation
{

class inner_product_estimator
{
public:

    typedef boost::shared_ptr<inner_product_estimator> ptr_t;

    typedef features::sparse_features features_t;

    inner_product_estimator()
     : _soft_prob_coeff(1.0)
    { reset(); }

    void reset()
    {
        _samples.clear();
        _sample_mass = 0;
        _std_dev = 0;

        _mean.clear();
    }

    void add_observation(
        const features_t::ptr_t & feat, double prob_class_sample, bool is_hard)
    {
        if(prob_class_sample <= 0)
            return;

        if(!is_hard)
            prob_class_sample *= _soft_prob_coeff;

        // add contribution into total amount of sample mass
        _sample_mass += prob_class_sample;

        // hang on to the sample; we'll need it for co-variance
        _samples.push_back(std::make_pair(feat, prob_class_sample));

        features::iterator<features_t>::type f_it, f_end;
        f_it = features::iterator<features_t>::begin(feat);
        f_end = features::iterator<features_t>::end(feat);

        double feat_norm = 0;

        for(; f_it != f_end; ++f_it)
        {
            double val = features::deref_value(f_it);
            feat_norm += val * val;
        }
        feat_norm = 1.0 / std::sqrt(feat_norm);

        f_it = features::iterator<features_t>::begin(feat);

        for(; f_it != f_end; ++f_it)
        {
            double val = features::deref_value(f_it) * prob_class_sample;
            _mean[features::deref_id(f_it)] += val * feat_norm;
        }
        return;
    }

    double prepare_estimator()
    {
        // normalize to obtain unit-length MLE mean
        double mean_norm = 0;
        for(mean_t::iterator it = _mean.begin(); it != _mean.end(); ++it)
            mean_norm += it->second * it->second;

        mean_norm = 1.0 / std::sqrt(mean_norm);
        for(mean_t::iterator it = _mean.begin(); it != _mean.end(); ++it)
            it->second *= mean_norm;

        // compute spherical std-dev
        double sq_std_dev = 0;
        for(unsigned i = 0; i != _samples.size(); ++i)
        {
            double dist = distance(_samples[i].first);
            sq_std_dev += dist * dist * _samples[i].second;
        }
        sq_std_dev /= _sample_mass;

        _pdf_norm = 1.0 / std::sqrt(2.0 * M_PI * sq_std_dev);
        _pdf_exp_norm = -1.0 / (2.0 * sq_std_dev);

        // not actually used
        _std_dev = std::sqrt(sq_std_dev);

        // return entropy of distribution
        return 0.5 * std::log(2.0 * M_PI * M_E * sq_std_dev);
    }

    double estimate(const features_t::ptr_t & feat)
    {
        double dist = distance(feat);
        return _pdf_norm * std::exp(_pdf_exp_norm * dist * dist);
    }

    double get_soft_prob_coeff()
    { return _soft_prob_coeff; }

    void set_soft_prob_coeff(double soft_prob_coeff)
    {
        if(soft_prob_coeff < 0 || soft_prob_coeff > 1)
            throw std::runtime_error("soft_prob_coeff range error");

        _soft_prob_coeff = soft_prob_coeff;
        return;
    }

    double inner_product(const features_t::ptr_t & feat)
    {
        features::iterator<features_t>::type f_it, f_end;
        f_it = features::iterator<features_t>::begin(feat);
        f_end = features::iterator<features_t>::end(feat);

        double feat_norm = 0, prod = 0;
        for(; f_it != f_end; ++f_it)
        {
            unsigned f_id = features::deref_id(f_it);
            double f_val  = features::deref_value(f_it);

            feat_norm += f_val * f_val;

            mean_t::const_iterator m_it = _mean.find(f_id);
            if(m_it == _mean.end())
                continue;

            prod += f_val * m_it->second;
        }
        return prod / std::sqrt(feat_norm);
    }

    double distance(const features_t::ptr_t & feat)
    {
        //return 1.0 - std::sqrt(inner_product(feat));

        // length of chord on unit sphere
        return 2 * std::sin(std::acos(inner_product(feat)) / 2.0);
    }

    boost::python::dict get_mean()
    {
        boost::python::dict d;
        for(mean_t::iterator it = _mean.begin(); it != _mean.end(); ++it)
            d[it->first] = it->second;

        return d;
    }

    double get_std_deviation()
    { return _std_dev; }

private:

    double _soft_prob_coeff;

    double _sample_mass;
    std::vector<std::pair<features_t::ptr_t, double> > _samples;

    typedef boost::unordered_map<unsigned, double> mean_t;
    mean_t _mean;
    
    double _std_dev;
    double _pdf_exp_norm;
    double _pdf_norm;
};

};

#endif

