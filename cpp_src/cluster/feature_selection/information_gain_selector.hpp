
#ifndef CLUSTER_FEATURE_SELECTION_INFORMATION_GAIN_MIXIN_HPP
#define CLUSTER_FEATURE_SELECTION_INFORMATION_GAIN_MIXIN_HPP

#include "cluster/feature_selection/maximum_likelihood_mixin.hpp"
#include <boost/shared_ptr.hpp>
#include <cmath>

namespace cluster
{
namespace feature_selection
{

template<typename Derived, typename InputFeatures>
class information_gain_mixin :
    public maximum_likelihood_mixin<InputFeatures>
{
public:

    typedef std::vector< std::pair<unsigned, double> > feature_stat_t;

    information_gain_mixin(
        double class_smoothing_factor)
     : maximum_likelihood_mixin<InputFeatures>(
        class_smoothing_factor)
    { }

    feature_stat_t derive_statistics(double & stat_sum)
    {
        Derived & self = static_cast<Derived&>(*this);

        // IG(C|F) = H(F) + H(C) - H(F & C)

        // H(C) = sum{ -P(c) * log P(c) }
        double igain_init = 0;
        for(unsigned i = 0; i != self._num_classes; ++i)
        {
            double prob_class = self._class_mass[i] / self._total_mass;
            igain_init += -prob_class * log(prob_class);
        }

        feature_stat_t feat_igain;

        for(typename vec_flt_map_t::iterator it = \
            self._feature_class_mass.begin();
            it != self._feature_class_mass.end(); ++it)
        {
            double igain = igain_init;

            // P(f)
            double prob_feat = self._feature_mass[it->first] / self._total_mass;

            // H(F) = -P(f) * log P(f) + -P(!f) * log P(!f)
            igain += -prob_feat * log(prob_feat);
            igain += -(1.0 - prob_feat) * log(1.0 - prob_feat);

            vec_flt_t & cur_class_mass = self._feature_class_mass[it->first];
            for(unsigned i = 0; i != self._num_classes; ++i)
            {
                // P(c)
                double prob_class = self._class_mass[i] / self._total_mass;

                // P(c & f)
                double prob_feat_class = cur_class_mass[i] / self._total_mass;

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

private:

    typedef maximum_likelihood_mixin<InputFeatures> ml_t;

    typedef typename ml_t::vec_flt_t vec_flt_t;
    typedef typename ml_t::vec_flt_map_t vec_flt_map_t;
    typedef typename ml_t::flt_map_t flt_map_t;
};

};
};

#endif

