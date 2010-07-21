#ifndef CLUSTER_FEATURE_TRANSFORM_RANDOM_PROJECTOR_HPP
#define CLUSTER_FEATURE_TRANSFORM_RANDOM_PROJECTOR_HPP

#include "cluster/features/dense_features.hpp"
#include "cluster/features/traits.hpp"
#include <boost/functional/hash.hpp>

namespace cluster {
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

        typedef features::dense_features result_type;

        features::value<result_type>::type operator()(
            typename features::reference<InputFeatures>::type ifeat)
        {
            typename result_type::mutable_ptr_t of_ptr(
                new result_type(self->_n_output_features));

            result_type & ofeat(*of_ptr);

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

}
#endif
