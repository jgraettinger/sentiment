
#ifndef CLUSTER_FEATURE_SELECTION_INFORMATION_GAIN_SELECTOR_HPP
#define CLUSTER_FEATURE_SELECTION_INFORMATION_GAIN_SELECTOR_HPP

#include "cluster/feature_selection/maximum_likelihood_mixin.hpp"
#include "cluster/feature_selection/selector_mixin.hpp"
#include "cluster/sparse_features.hpp"
#include <boost/shared_ptr.hpp>
#include <cmath>

namespace cluster
{
namespace feature_selection
{

class information_gain_selector;

class information_gain_selector :
    public selector_mixin<information_gain_selector>,
    public maximum_likelihood_mixin<sparse_features>
{
public:

    typedef boost::shared_ptr<information_gain_selector> ptr_t;

    typedef std::vector< std::pair<unsigned, double> > feature_stat_t;

    information_gain_selector(unsigned max_features, double max_mass_ratio)
     : selector_mixin<information_gain_selector>(
        max_features, max_mass_ratio)
    { }

    feature_stat_t derive_statistics(double & stat_sum)
    {
        // IG(C|F) = H(F) + H(C) - H(F & C)

        // H(C) = sum{ -P(c) * log P(c) }
        double igain_init = 0;
        for(unsigned i = 0; i != _num_classes; ++i)
        {
            double prob_class = _class_mass[i] / _total_mass;
            igain_init += -prob_class * log(prob_class);
        }

        feature_stat_t feat_igain;

        for(vec_flt_map_t::iterator it = _feature_class_mass.begin();
            it != _feature_class_mass.end(); ++it)
        {
            double igain = igain_init;

            // P(f)
            double prob_feat = _feature_mass[it->first] / _total_mass;

            // H(F) = -P(f) * log P(f) + -P(!f) * log P(!f)
            igain += -prob_feat * log(prob_feat);
            igain += -(1.0 - prob_feat) * log(1.0 - prob_feat);

            vec_flt_t & cur_class_mass = _feature_class_mass[it->first];
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

            feat_igain.push_back( std::make_pair(it->first, igain));
            stat_sum += igain;
        }

        return feat_igain;
    }
};

};
};

#endif

