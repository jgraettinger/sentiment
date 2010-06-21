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
        _num_classes = 0;
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
        if(!_num_classes)
        {
            _num_classes = class_weight.size();
            _class_mass.resize(class_weight.size(), 0);
        }

        if(class_weight.size() != _num_classes)
            throw std::runtime_error("class arity mismatch");

        // We're collecting weighted counts for three maximum-
        //  likilihood estimates here:
        //   p(c)     (generative probability of a class)
        //   p(f)     (generative probability of a feature)
        //   p(c & f) (generative probability of a class label & feature)

        double class_mass = std::accumulate(
            class_weight.begin(), class_weight.end(), 0.0);

        double feature_mass = 0;

        // collect observations for p(c & f)
        for(features_t::const_iterator it = feat->begin();
            it != feat->end(); ++it)
        {
            vec_t & cur_class_mass = _feature_class_mass[it->first];

            if(cur_class_mass.empty())
                cur_class_mass.resize(_num_classes, 0);

            for(unsigned i = 0; i != _num_classes; ++i)
                cur_class_mass[i] += it->second * class_weight[i];

            feature_mass += it->second;
        }

        // collect observations for p(c)
        for(unsigned i = 0; i != _num_classes; ++i)
        {
            _class_mass[i] += class_weight[i] * feature_mass;
        }

        // collect observations for p(f)
        for(features_t::const_iterator it = feat->begin();
            it != feat->end(); ++it)
        {
            _feature_mass[it->first] += it->second * class_mass;
        }

        // normalizing constant
        _total_mass += class_mass * feature_mass;
        return;
    }

    void sanity()
    {
        std::cout << "total_mass " << _total_mass << std::endl;

        {
            double norm = 0;
            for(vec_map_t::const_iterator it1 = _feature_class_mass.begin();
                it1 != _feature_class_mass.end(); ++it1)
            {
                for(vec_t::const_iterator it2 = it1->second.begin();
                    it2 != it1->second.end(); ++it2)
                {
                    norm += *it2;
                }
            }
            std::cout << "feat_class_mass " << norm << std::endl;
        }

        {
            double norm = 0;
            for(unsigned i = 0; i != _num_classes; ++i)
            {
                norm += _class_mass[i];
            }
            std::cout << "class_mass " << norm << std::endl;
        }

        {
            double norm = 0;
            for(double_map_t::const_iterator it = _feature_mass.begin();
                it != _feature_mass.end(); ++it)
            {
                norm += it->second; 
            }
            std::cout << "class_mass " << norm << std::endl;
        }
    }

    double_map_t get_information_gain()
    {
        // IG(C|F) = H(F) + H(C) - H(F & C)

        // H(C) = sum{ -P(c) * log P(c) }
        double igain_init = 0;
        for(unsigned i = 0; i != _num_classes; ++i)
        {
            double prob_class = _class_mass[i] / _total_mass;
            igain_init += -prob_class * log(prob_class);
        }

        double_map_t feat_igain;

        for(vec_map_t::iterator it = _feature_class_mass.begin();
            it != _feature_class_mass.end(); ++it)
        {
            double igain = igain_init;

            // P(f)
            double prob_feat = _feature_mass[it->first] / _total_mass;

            // H(F) = -P(f) * log P(f) + -P(!f) * log P(!f)
            igain += -prob_feat * log(prob_feat);
            igain += -(1.0 - prob_feat) * log(1.0 - prob_feat);

            vec_t & cur_class_mass = _feature_class_mass[it->first];
            for(unsigned i = 0; i != _num_classes; ++i)
            {
                // P(c)
                double prob_class = _class_mass[i] / _total_mass;

                // P(c & f)
                double prob_feat_class = cur_class_mass[i] / _total_mass;

                // P(c & !f) = P(c) - P(c & f)
                double prob_not_feat_class = prob_class - prob_feat_class;

                // H(F, C) = sum {
                //          -P(c & f)  * log P(c & f) +
                //          -P(c & !f) * log P(c & !f)
                //      } for c in C
                igain -= -prob_feat_class * log(prob_feat_class);
                igain -= -prob_not_feat_class * log(prob_not_feat_class);
            }

            feat_igain[it->first] = igain;
        }

        return feat_igain;
    }

private:

    vec_t _class_mass;

    double_map_t _feature_mass;

    vec_map_t _feature_class_mass;

    unsigned _num_classes;
    double _total_mass;
};

};

#endif
