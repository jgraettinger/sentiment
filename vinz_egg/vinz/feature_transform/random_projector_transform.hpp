#ifndef FEATURE_TRANSFORM_RANDOM_PROJECTOR_HPP
#define FEATURE_TRANSFORM_RANDOM_PROJECTOR_HPP

#include "features/dense_features.hpp"
#include "features/traits.hpp"
#include <boost/functional/hash.hpp>

namespace feature_transform {

class random_projector_transform
{
public:

    typedef boost::shared_ptr<random_projector_transform> ptr_t;

    random_projector_transform(unsigned n_output_features)
     : _root_3(std::sqrt(3.0)),
       _n_output_features(n_output_features) 
    { }

    template<typename InputFeatures>
    struct train_transform
    {
        train_transform(ptr_t){}

        typedef InputFeatures input_features_t;

        template<typename FeaturesMembershipsZipIterator>
        void operator()(
            const FeaturesMembershipsZipIterator & begin,
            const FeaturesMembershipsZipIterator & end)
        {

        }
    };

    template<typename InputFeatures>
    struct transform
    {
        ptr_t self;
        transform(const ptr_t & self) : self(self){}

        typedef InputFeatures input_features_t;
        typedef features::dense_features output_features_t;
        typedef features::value<output_features_t>::type result_type;

        result_type operator()(
            typename features::reference<InputFeatures>::type ifeat) const
        {
            typename output_features_t::mutable_ptr_t of_ptr(
                new output_features_t(self->_n_output_features));

            output_features_t & ofeat(*of_ptr);

            typename features::iterator<InputFeatures>::type it = \
                features::begin<InputFeatures>(ifeat);
            typename features::iterator<InputFeatures>::type end = \
                features::end<InputFeatures>(ifeat);

            for(; it != end; ++it)
            {
                for(unsigned i = 0; i != self->_n_output_features; ++i)
                {
                    size_t h = 0;

                    boost::hash_combine(h, features::deref_id(it));
                    boost::hash_combine(h, i);

                    h = h % 6;

                    if(h == 0)
                        ofeat[i] += self->_root_3 * features::deref_value(it);
                    if(h == 1)
                        ofeat[i] -= self->_root_3 * features::deref_value(it);
                }
            }

            return of_ptr;
        }
    };

private:

    const double _root_3;
    const unsigned _n_output_features;
};

}

#endif
