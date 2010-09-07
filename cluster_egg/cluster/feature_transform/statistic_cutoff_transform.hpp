#ifndef CLUSTER_FEATURE_TRANSFORM_STATISTIC_CUTOFF_TRANSFORM_HPP
#define CLUSTER_FEATURE_TRANSFORM_STATISTIC_CUTOFF_TRANSFORM_HPP

#include "cluster/features/sparse_features.hpp"
#include "cluster/features/traits.hpp"
#include <boost/unordered_set.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>

namespace cluster {
namespace feature_transform {

template<typename StatisticProvider>
class statistic_cutoff_transform :
    public StatisticProvider
{
public:

    typedef boost::shared_ptr<statistic_cutoff_transform> ptr_t;

    typedef StatisticProvider statistic_provider_t;
    typedef typename statistic_provider_t::feature_stat_t feature_stat_t;

    statistic_cutoff_transform(
        unsigned min_features,
        double max_mass_ratio,
        unsigned max_features,
        const statistic_provider_t & stat_provider)
     : 
        statistic_provider_t(stat_provider),
        _min_features(min_features),
        _max_mass_ratio(max_mass_ratio),
        _max_features(max_features)
    {
        std::cout << "min-feat " << _min_features << std::endl;
        std::cout << "max-feat " << _max_features << std::endl;
        std::cout << "max-ratio " << _max_mass_ratio << std::endl;
    }

    template<typename InputFeatures>
    struct train_transform
    {
        ptr_t self;
        train_transform(const ptr_t & self) : self(self) {}

        typedef InputFeatures input_features_t;

        template<typename FeaturesMembershipsZipIterator>
        feature_stat_t select_features(
            FeaturesMembershipsZipIterator it,
            const FeaturesMembershipsZipIterator & end)
        {
            self->reset();

            for(; it != end; ++it)
            {
                self->template add_observation<InputFeatures>(
                    it->template get<0>(), it->template get<1>());
            }

            double stat_sum = 0;
            feature_stat_t feat_stats = self->derive_statistics(stat_sum);

            std::cout << "STAT-CUTOFF: " << feat_stats.size() << " input features, ";
            std::cout << stat_sum << " stat sum, ";

            double   cur_sum = 0;
            unsigned cur_count = 0;

            unsigned max_features = std::min(
                self->_max_features, feat_stats.size());

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
                    if(cur_count >= self->_min_features && \
                      (cur_sum / stat_sum) > self->_max_mass_ratio)
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

        template<typename FeaturesMembershipsZipIterator>
        void operator()(
            const FeaturesMembershipsZipIterator & begin,
            const FeaturesMembershipsZipIterator & end)
        {
            feature_stat_t feat_stats = select_features(begin, end);

            std::cout << feat_stats.size() << " output features" << std::endl;
            
            // insert selected feature-id's into feature index
            self->_feat_index.clear();
            for(typename feature_stat_t::const_iterator it = feat_stats.begin();
                it != feat_stats.end(); ++it)
            {
                self->_feat_index.insert(it->first);
            }
            return;
        }
    };

    template<typename InputFeatures>
    struct transform
    {
        ptr_t self;
        transform(const ptr_t & self) : self(self) {}

        typedef InputFeatures input_features_t;
        typedef features::sparse_features output_features_t;
        typedef features::value<output_features_t>::type result_type;

        result_type operator()(
            typename features::reference<InputFeatures>::type ifeat) const
        {
            typename output_features_t::mutable_ptr_t of_ptr(
                new output_features_t());

            output_features_t & ofeat(*of_ptr);

            typename features::iterator<InputFeatures>::type it = \
                features::begin<InputFeatures>(ifeat);
            typename features::iterator<InputFeatures>::type end = \
                features::end<InputFeatures>(ifeat);

            for(; it != end; ++it)
            {
                unsigned f_id = features::deref_id(it);
                double  f_val = features::deref_value(it);

                if(self->_feat_index.find(f_id) == self->_feat_index.end())
                    continue;

                ofeat.push_back( std::make_pair(f_id, f_val));
            }

            std::sort(ofeat.begin(), ofeat.end());
            return of_ptr;
        }
    };

private:

    struct statistic_cmp
    {
        bool operator ()(
            const typename feature_stat_t::value_type & f1, 
            const typename feature_stat_t::value_type & f2) const
        { return f1.second > f2.second; }
    };

    unsigned _min_features;
    double _max_mass_ratio;
    unsigned _max_features;

    boost::unordered_set<unsigned> _feat_index;
};

};
};

#endif

