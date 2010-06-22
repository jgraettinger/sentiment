
#ifndef CLUSTER_FEATURE_SELECTION_SELECTOR_MIXIN_HPP
#define CLUSTER_FEATURE_SELECTION_SELECTOR_MIXIN_HPP

#include <vector>

namespace cluster
{
namespace feature_selection
{

template<typename Derived>
class selector_mixin
{
public:

    typedef std::vector< std::pair<unsigned, double> > feature_stat_t;

    selector_mixin(
        unsigned min_features,
        double max_mass_ratio,
        unsigned max_features)
     : 
        _min_features(min_features),
        _max_mass_ratio(max_mass_ratio),
        _max_features(max_features)
    { }

    feature_stat_t select_features()
    {
        double stat_sum = 0;
        feature_stat_t feat_stats = \
            static_cast<Derived*>(this)->derive_statistics(stat_sum);

        double   cur_sum = 0;
        unsigned cur_count = 0;

        unsigned max_features = std::min(
            _max_features, feat_stats.size());

        while(cur_count != max_features)
        {
            unsigned next_count = std::min(
                max_features, cur_count + 100);

            std::partial_sort(
                feat_stats.begin() + cur_count,
                feat_stats.begin() + next_count,
                feat_stats.end(),
                statistic_cmp()
            );

            while(cur_count != next_count)
            {
                cur_sum += feat_stats[cur_count++].second;
                if(cur_count >= _min_features && \
                  (cur_sum / stat_sum) > _max_mass_ratio)
                {
                    // we've reached the target ratio
                    feat_stats.erase(
                        feat_stats.begin() + cur_count, feat_stats.end());
                    return feat_stats;
                }
            }
        }

        // we've reached max_features, or feat_stats.size()
        feat_stats.erase(
            feat_stats.begin() + cur_count, feat_stats.end());
        return feat_stats;
    }

private:

    struct statistic_cmp
    {
        bool operator ()(
            const feature_stat_t::value_type & f1, 
            const feature_stat_t::value_type & f2) const
        { return f1.second > f2.second; }
    };

    unsigned _min_features;
    double _max_mass_ratio;
    unsigned _max_features;
};

};
};

#endif

