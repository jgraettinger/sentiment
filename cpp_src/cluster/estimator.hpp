
#ifndef ESTIMATOR_HPP
#define ESTIMATOR_HPP

#include "cluster/sample.hpp"
#include "cluster/featurizer.hpp"
#include <boost/shared_ptr.hpp>

namespace cluster
{

class estimator
{
public:

    typedef boost::shared_ptr<estimator> ptr_t;

    estimator(const featurizer::ptr_t & f)
     : _featurizer(f)
    { }

    featurizer::ptr_t get_featurizer()
    { return _featurizer; }

    // sample interface

    sample_features::ptr_t add_sample(const sample::ptr_t & s)
    { return _featurizer->featurize(s); }

    void drop_sample(const sample::ptr_t & s)
    {
        cached_featurizer::ptr_t f(
            boost::dynamic_pointer_cast<cached_featurizer>(_featurizer));

        if(f) f->drop(s);
    }

    /// Estimator interface

    // Prepare for a sequence of calls to add_sample_probability
    virtual void reset_estimator() = 0;

    // train
    virtual void add_sample_probability(
        const sample_features::ptr_t & f, double prob_class_sample) = 0;

    virtual void prepare_estimator() = 0;

    // decode prob(sample | class)
    virtual double estimate_sample(const sample_features::ptr_t & f) = 0;

protected:

    featurizer::ptr_t _featurizer;
};

};

#endif
