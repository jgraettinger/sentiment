#ifndef CLUSTER_INFORMATION_GAIN_SELECTOR
#define CLUSTER_INFORMATION_GAIN_SELECTOR

#include "cluster/sparse_features.hpp"
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <numeric>
#include <vector>
#include <cmath>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <iostream>

namespace cluster
{

class information_gain_selector
{
public:

    typedef boost::shared_ptr<information_gain_selector> ptr_t;

    typedef sparse_features features_t;

    typedef std::vector<double> vec_t;
    typedef boost::unordered_map<unsigned, vec_t> vec_map_t;
    typedef boost::unordered_map<unsigned, double> double_map_t;

    information_gain_selector()
    { reset(); }

    void reset()
    {
        _total_mass = 0;
        _class_mass.clear();
        _feature_mass.clear();
        _feature_class_mass.clear();
    }

    void add_observation(
        const features_t::ptr_t & feat,
        const vec_t & class_weight)
    {
        // first sample?
        if(_class_mass.empty())
            _class_mass.resize(class_weight.size(), 0);

        if(_class_mass.size() != class_weight.size())
            throw std::runtime_error("arity mismatch");

        // We're collecting three maximum-likilihood estimates here:
        //   p(c)   (generative probability of a class)
        //   p(f)   (generative probability of a feature)
        //   p(c & f) (generative probability of a class label & feature)

        double feature_mass = 0;

        // collect observations for p(c & f)
        for(features_t::const_iterator it = feat->begin();
            it != feat->end(); ++it)
        {
            vec_t & cur_class_mass = _feature_class_mass[it->first];

            if(cur_class_mass.empty())
                cur_class_mass.resize(_class_mass.size(), 0);

            for(unsigned i = 0; i != _class_mass.size(); ++i)
                cur_class_mass[i] += it->second * class_weight[i];

            feature_mass += it->second;
        }

        double class_mass = std::accumulate(
            class_weight.begin(), class_weight.end(), 0.0);

        // collect observations for p(c)
        for(unsigned i = 0; i != _class_mass.size(); ++i)
        {
            _class_mass[i] += class_weight[i] * feature_mass;
        }

        // collect observations for p(f)
        for(features_t::const_iterator it = feat->begin();
            it != feat->end(); ++it)
        {
            _feature_mass[it->first] += it->second * class_mass;
        }

        _total_mass += class_mass * feature_mass;
        return;
    }

    double_map_t get_information_gain()
    {
        // compute sum{ -P(c) * log P(c) }
        double igain_init = 0;
        for(unsigned i = 0; i != _class_mass.size(); ++i)
        {
            double prob_class = _class_mass[i] / _total_mass;
            igain_init += -prob_class * log(prob_class);
        }

        std::cout << "igain-init: " << igain_init << std::endl;

        double_map_t feat_igain;

        for(vec_map_t::iterator it = _feature_class_mass.begin();
            it != _feature_class_mass.end(); ++it)
        {
            double igain = igain_init;

            // P(f)
            double prob_feat = _feature_mass[it->first] / _total_mass;

            std::cout << "prob " << it->first << ": " << prob_feat << std::endl;

            vec_t & cur_class_mass = _feature_class_mass[it->first];
            for(unsigned i = 0; i != cur_class_mass.size(); ++i)
            {
                // P(c)
                double prob_class = _class_mass[i] / _total_mass;

                // P(c && f)
                double prob_feat_class = cur_class_mass[i] / _total_mass;

                // P(c | f) = P(c && f) / P(f)
                double cond_prob_class_feat = prob_feat_class / prob_feat;

                // P(c | !f) = P(c) - P(c && f)
                double cond_prob_class_not_feat = prob_class - prob_feat_class;

                std::cout << "p(" << i << " |  " << it->first << "): " << cond_prob_class_feat << std::endl;
                std::cout << "p(" << i << " | !" << it->first << "): " << cond_prob_class_not_feat << std::endl;

                // igain += P(f) * P(c | f) * log P(c | f)
                igain += prob_feat * cond_prob_class_feat * log(cond_prob_class_feat);

                // igain += P(!f) * P(c | !f) * log P(c | !f)
                igain += (1.0 - prob_feat) * cond_prob_class_not_feat * log(cond_prob_class_not_feat);
            }

            feat_igain[it->first] = igain;
        }

        return feat_igain;
    }

private:

    vec_t _class_mass;

    double_map_t _feature_mass;

    vec_map_t _feature_class_mass;

    double _total_mass;
};

};

#endif
