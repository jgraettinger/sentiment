#ifndef CLUSTER_FEATURE_TRANSFORM_MAXIMUM_LIKELIHOOD_MIXIN_HPP
#define CLUSTER_FEATURE_TRANSFORM_MAXIMUM_LIKELIHOOD_MIXIN_HPP

#include "cluster/features/traits.hpp"
#include <boost/unordered_map.hpp>
#include <vector>
#include <numeric>
#include <cfloat>

namespace cluster {
namespace feature_transform {

class maximum_likelihood_mixin
{
public:

    typedef std::vector<double> vec_flt_t;
    typedef boost::unordered_map<unsigned, vec_flt_t> vec_flt_map_t;
    typedef boost::unordered_map<unsigned, double> flt_map_t;

    maximum_likelihood_mixin(double class_smoothing_factor)
     : _smoothing(class_smoothing_factor)
    { reset(); }

    void reset()
    {
        _num_classes = 0;
        _total_mass = 0;
        _class_mass.clear();
        _feature_mass.clear();
        _feature_class_mass.clear();
    }

    template<typename Features>
    void add_observation(
        typename features::reference<Features>::type feat,
        vec_flt_t class_mass_vec)
    {
        // first sample?
        if(!_num_classes)
        {
            _num_classes = class_mass_vec.size();
            _class_mass.resize(class_mass_vec.size(), 0);
        }

        if(class_mass_vec.size() != _num_classes)
            throw std::runtime_error("class arity mismatch");

        // We're collecting weighted counts for three maximum-
        //  likilihood estimates here:
        //   p(c)     (generative probability of a class)
        //   p(f)     (generative probability of a feature)
        //   p(c & f) (generative probability of a class label & feature)

        double class_mass = std::accumulate(
            class_mass_vec.begin(), class_mass_vec.end(), 0.0);

        if(class_mass < FLT_EPSILON)
            return;

        // smooth observed class masses by degree '_smoothing'
        {
            double avg_class_mass = class_mass / _num_classes;

            for(unsigned i = 0; i != _num_classes; ++i)
            {
                class_mass_vec[i] += _smoothing * (
                    avg_class_mass - class_mass_vec[i]);
            }
        }

        double feature_mass = 0;

        typename features::iterator<Features>::type it, end;
        it = features::begin<Features>(feat);
        end = features::end<Features>(feat);

        // collect observations for p(c & f) & p(f)
        for(; it != end; ++it)
        {
            unsigned f_id = features::deref_id(it);
            double  f_val = features::deref_value(it);

            vec_flt_t & cur_feat_class_mass = _feature_class_mass[f_id];

            if(cur_feat_class_mass.empty())
                cur_feat_class_mass.resize(_num_classes, 0);

            // p(c & f)
            for(unsigned i = 0; i != _num_classes; ++i)
                cur_feat_class_mass[i] += f_val * class_mass_vec[i];

            // p(f)
            _feature_mass[f_id] += f_val * class_mass;

            feature_mass += f_val;
        }

        // collect observations for p(c)
        for(unsigned i = 0; i != _num_classes; ++i)
        {
            _class_mass[i] += class_mass_vec[i] * feature_mass;
        }

        // normalizing constant
        _total_mass += class_mass * feature_mass;
        return;
    }

protected:

    unsigned _num_classes;

    vec_flt_t _class_mass;
    flt_map_t _feature_mass;
    vec_flt_map_t _feature_class_mass;

    double _total_mass;

    double _smoothing;
};

};
};

#endif

