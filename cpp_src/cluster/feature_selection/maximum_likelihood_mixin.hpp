
#ifndef CLUSTER_FEATURE_SELECTION_MAXIMUM_LIKELIHOOD_MIXIN_HPP
#define CLUSTER_FEATURE_SELECTION_MAXIMUM_LIKELIHOOD_MIXIN_HPP

#include <boost/unordered_map.hpp>
#include <vector>
#include <numeric>
#include <cfloat>


namespace cluster
{
namespace feature_selection
{

template<typename Features>
class maximum_likelihood_mixin
{
public:

    typedef Features features_t;

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

    void add_observation(
        const typename features_t::ptr_t & fptr,
        vec_flt_t class_mass_vec)
    {
        const features_t & feat(*fptr);

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

        // collect observations for p(c & f)
        for(typename features_t::const_iterator it = feat.begin();
            it != feat.end(); ++it)
        {
            vec_flt_t & cur_feat_class_mass = _feature_class_mass[it->first];

            if(cur_feat_class_mass.empty())
                cur_feat_class_mass.resize(_num_classes, 0);

            for(unsigned i = 0; i != _num_classes; ++i)
                cur_feat_class_mass[i] += it->second * class_mass_vec[i];

            feature_mass += it->second;
        }

        // collect observations for p(c)
        for(unsigned i = 0; i != _num_classes; ++i)
        {
            _class_mass[i] += class_mass_vec[i] * feature_mass;
        }

        // collect observations for p(f)
        for(typename features_t::const_iterator it = feat.begin();
            it != feat.end(); ++it)
        {
            _feature_mass[it->first] += it->second * class_mass;
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

