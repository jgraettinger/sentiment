#ifndef CLUSTER_FEATURE_TRANSFORM_CHAINED_TRANSFORM_HPP
#define CLUSTER_FEATURE_TRANSFORM_CHAINED_TRANSFORM_HPP

#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

namespace cluster {
namespace feature_transform {

template<typename FirstTransform, typename SecondTransform>
class chained_transform
{
public:

    typedef boost::shared_ptr<chained_transform> ptr_t;

    typedef FirstTransform  first_transform_t;
    typedef SecondTransform second_transform_t;

    chained_transform(
        typename first_transform_t::ptr_t first,
        typename second_transform_t::ptr_t second)
     : _first(first), _second(second)
    { }

    template<typename InputFeatures>
    struct train_transform
    {
        ptr_t self;
        train_transform(const ptr_t & self) : self(self) {}

        template<typename FeaturesMembershipsZipIterator>
        void operator()(
            const FeaturesMembershipsZipIterator & begin,
            const FeaturesMembershipsZipIterator & end)
        {
            typedef typename first_transform_t::template train_transform<
                InputFeatures> first_train_t;
            typedef typename first_transform_t::template transform<
                InputFeatures> first_transform_t;
            typedef typename second_transform_t::template train_transform<
                typename first_transform_t::result_type> second_train_t;

            // train first transform
            first_train_t(self->_first)(begin, end);

            // train second transform over features
            //   transformed by first transform
            second_train_t(self->_second)(
                boost::make_zip_iterator( boost::make_tuple(
                        boost::make_transform_iterator(
                            begin.get_iterator_tuple().template get<0>(),
                            first_transform_t(self->_first)),
                        begin.get_iterator_tuple().template get<1>())),
                boost::make_zip_iterator(
                    boost::make_tuple(
                        boost::make_transform_iterator(
                            end.get_iterator_tuple().template get<0>(),
                            first_transform_t(self->_first)),
                        end.get_iterator_tuple().template get<1>())));
        };
    };

    template<typename InputFeatures>
    struct transform
    {
        ptr_t self;
        transform(const ptr_t & self) : self(self) {}

        typedef typename first_transform_t::template transform<InputFeatures
            >::result_type first_result_t;
        typedef typename second_transform_t::template transform<first_result_t
            >::result_type result_type;

        typename features::value<result_type>::type operator()(
            typename features::reference<InputFeatures>::type feat)
        {
            return typename second_transform_t::template transform<first_result_t
                >(self->_second)(
                    typename first_transform_t::template transform<InputFeatures
                    >(self->_first)(feat));
        }
    };

private:

    typename first_transform_t::ptr_t  _first;
    typename second_transform_t::ptr_t _second;
};

};
};

#endif

