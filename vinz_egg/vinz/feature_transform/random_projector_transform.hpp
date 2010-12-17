#ifndef FEATURE_TRANSFORM_RANDOM_PROJECTOR_HPP
#define FEATURE_TRANSFORM_RANDOM_PROJECTOR_HPP

#include "features/dense_features.hpp"
#include "features/traits.hpp"
#include <boost/functional/hash.hpp>
#include <boost/unordered_map.hpp>
#include <boost/python.hpp>
#include <iostream>

namespace feature_transform {

class random_projector_transform
{
public:

    typedef boost::shared_ptr<random_projector_transform> ptr_t;

    random_projector_transform(unsigned n_output_features)
     : _root_3(std::sqrt(3.0)),
       _n_output_features(n_output_features),
       _rand("/dev/urandom")
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
            ofeat.fill(0);

            typename features::iterator<InputFeatures>::type it = \
                features::begin<InputFeatures>(ifeat);
            typename features::iterator<InputFeatures>::type end = \
                features::end<InputFeatures>(ifeat);

            for(; it != end; ++it)
            {
                unsigned f_id = features::deref_id(it);
                double f_val = features::deref_value(it);

                for(unsigned i = 0; i != self->_n_output_features; ++i)
                {
                    std::pair<unsigned, unsigned> key(f_id, i);

                    typename mat_r_t::iterator m_it(self->_mat_r.find(key));

                    if(m_it == self->_mat_r.end())
                    {
                        unsigned t = self->_rand.get() % 6;
                        double m_r_val = self->_root_3;

                        if(t == 0)
                            m_r_val *= 1;
                        else if(t == 1)
                            m_r_val *= -1;
                        else
                            m_r_val = 0;

                        m_it = self->_mat_r.insert(
                            std::make_pair(key, m_r_val)).first;
                    }

                    ofeat[i] += m_it->second * f_val;
                }
            }

            return of_ptr;
        }
    };

    boost::python::dict get_mat_r()
    {
        boost::python::dict d;
        for(mat_r_t::const_iterator it = _mat_r.begin(); it != _mat_r.end(); ++it)
        {
            d[boost::python::make_tuple(it->first.first, it->first.second)] = it->second;
        }
        return d;
    }

private:

    typedef boost::unordered_map<std::pair<unsigned, unsigned>, double> mat_r_t;
    mat_r_t _mat_r;

    const double _root_3;
    const unsigned _n_output_features;

    std::ifstream _rand;
};

}

#endif
