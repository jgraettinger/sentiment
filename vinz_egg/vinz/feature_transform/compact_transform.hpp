#ifndef FEATURE_TRANSFORM_COMPACT_TRANSFORM_HPP
#define FEATURE_TRANSFORM_COMPACT_TRANSFORM_HPP

#include "features/dense_features.hpp"
#include "features/traits.hpp"
#include <boost/unordered_map.hpp>

namespace feature_transform {

class compact_transform
{
public:

    typedef boost::shared_ptr<compact_transform> ptr_t;

    compact_transform() {}

    template<typename InputFeatures>
    struct train_transform
    {
        ptr_t self;
        train_transform(const ptr_t & self) : self(self) {}

        typedef InputFeatures input_features_t;

        template<typename FeaturesMembershipsZipIterator>
        void operator()(
            FeaturesMembershipsZipIterator s_it,
            const FeaturesMembershipsZipIterator & s_end)
        {
            self->_mapping.clear();
            self->_next_id = 0;

            for(; s_it != s_end; ++s_it)
            {
                typename features::reference<InputFeatures>::type feat(
                    boost::get<0>(*s_it));

                typename features::iterator<InputFeatures>::type it, end;
                it = features::begin<InputFeatures>(feat);
                end = features::end<InputFeatures>(feat);

                for(; it != end; ++it)
                {
                    unsigned f_id = features::deref_id(it);
                    double  f_val = features::deref_value(it);

                    mapping_t::iterator m_it = self->_mapping.find(f_id);

                    if(m_it == self->_mapping.end())
                    {
                        self->_mapping[f_id] = std::make_pair(0, f_val);
                    }
                    else if(m_it->second.second == m_it->second.second && \
                        m_it->second.second != f_val)
                    {
                        // first different value for feature we've seen
                        m_it->second.first = self->_next_id++;
                        m_it->second.second = std::numeric_limits<double>::quiet_NaN();
                    }
                }
            }
        }
    };

    template<typename InputFeatures>
    struct transform
    {
        ptr_t self;
        transform(const ptr_t & self) : self(self) {}

        typedef InputFeatures input_features_t;
        typedef features::dense_features output_features_t;
        typedef features::value<output_features_t>::type result_type;

        result_type operator()(
            typename features::reference<InputFeatures>::type ifeat) const
        {
            typename output_features_t::mutable_ptr_t of_ptr(
                new output_features_t(self->_next_id));

            output_features_t & ofeat(*of_ptr);

            typename features::iterator<InputFeatures>::type it = \
                features::begin<InputFeatures>(ifeat);
            typename features::iterator<InputFeatures>::type end = \
                features::end<InputFeatures>(ifeat);

            for(; it != end; ++it)
            {
                unsigned f_id = features::deref_id(it);
                double  f_val = features::deref_value(it);

                mapping_t::const_iterator m_it = self->_mapping.find(f_id);

                if(m_it != self->_mapping.end() && \
                    m_it->second.second != m_it->second.second)
                {
                    ofeat[m_it->second.first] = f_val;
                }
            }
            return of_ptr;
        }
    };

private:

    typedef boost::unordered_map<unsigned, std::pair<unsigned, double> > mapping_t;
    mapping_t _mapping;
    unsigned _next_id;
};

};

#endif
