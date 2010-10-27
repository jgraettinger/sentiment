#ifndef FEATURE_TRANSFORM_IDF_TRANSFORM_HPP
#define FEATURE_TRANSFORM_IDF_TRANSFORM_HPP

#include "features/sparse_features.hpp"
#include "features/traits.hpp"
#include <boost/functional/hash.hpp>
#include <boost/unordered_map.hpp>

namespace feature_transform {

class idf_transform
{
public:

    typedef boost::shared_ptr<idf_transform> ptr_t;

    idf_transform(
        double max_df_threshold,
        unsigned min_df_count)
     : _max_df_threshold(max_df_threshold),
       _min_df_count(min_df_count)
    {
        if(_max_df_threshold <= 0 || _max_df_threshold > 1.0)
            throw std::range_error("max_df_threshold range");
    }

    template<typename InputFeatures>
    struct train_transform
    {
        ptr_t self;
        train_transform(const ptr_t & self) : self(self) {}

        typedef InputFeatures input_features_t;

        template<typename FeaturesMembershipsZipIterator>
        void operator()(
            const FeaturesMembershipsZipIterator & begin,
            const FeaturesMembershipsZipIterator & end)
        {
            self->_df.clear();

            double sample_count = 0;

            for(FeaturesMembershipsZipIterator it = begin; it != end; ++it)
            {
                typename features::reference<InputFeatures>::type feat(
                    boost::get<0>(*it));

                typename features::iterator<InputFeatures>::type f_it, f_end;
                f_it = features::begin<InputFeatures>(feat);
                f_end = features::end<InputFeatures>(feat);

                for(; f_it != f_end; ++f_it)
                {
                    if(features::deref_value(f_it))
                        self->_df[features::deref_id(f_it)] += 1.0;
                }

                sample_count += 1;
            }

            // filter terms occuring in too high a percentage of
            //  documents (max_df_threshold), or not occuring in
            //  enough (min_df_count). Shape weights of the
            //  remaining term-space by log-IDF.
            feature_hash_t::iterator it = self->_df.begin();
            while(it != self->_df.end())
            {
                if(it->second >= self->_min_df_count &&
                  (it->second / sample_count) <= self->_max_df_threshold)
                {
                    it->second = -1.0 * std::log(it->second / sample_count);
                    ++it;
                }
                else
                {
                    // term is to be filtered
                    feature_hash_t::iterator t_it = it++;
                    self->_df.erase(t_it);
                }
            }
            return;
        }
    };

    template<typename InputFeatures>
    struct transform
    {
        ptr_t self;
        transform(const ptr_t & self) : self(self){}

        typedef InputFeatures input_features_t;
        typedef features::sparse_features output_features_t;
        typedef features::value<output_features_t>::type result_type;

        result_type operator()(
            typename features::reference<InputFeatures>::type ifeat) const
        {
            typename output_features_t::mutable_ptr_t of_ptr(
                new output_features_t());

            output_features_t & ofeat(*of_ptr);
            ofeat.reserve(features::size<InputFeatures>(ifeat));

            typename features::iterator<InputFeatures>::type it = \
                features::begin<InputFeatures>(ifeat);
            typename features::iterator<InputFeatures>::type end = \
                features::end<InputFeatures>(ifeat);

            for(; it != end; ++it)
            {
                unsigned f_id = features::deref_id(it);
                double f_val = features::deref_value(it);

                feature_hash_t::const_iterator df_it = self->_df.find(f_id);

                // filtered or unobserved term
                if(df_it == self->_df.end())
                    continue;

                ofeat.push_back(std::make_pair(f_id, f_val * df_it->second));
            }

            std::sort(ofeat.begin(), ofeat.end());
            ofeat.normalize_L2();
            return of_ptr;
        }
    };

private:

    double _max_df_threshold;
    unsigned _min_df_count;

    typedef boost::unordered_map<unsigned, double> feature_hash_t;
    feature_hash_t _df;
};

};

#endif

