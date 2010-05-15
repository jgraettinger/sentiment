
#ifndef ESTIMATOR_HPP
#define ESTIMATOR_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace cluster
{

class base_estimator
{
public:

    typedef boost::shared_ptr<base_estimator> ptr_t;

    class item_features
    {
    public:
        typedef boost::intrusive_ptr<item_features> ptr_t;

        item_features()
         : _ref_cnt(0)
        { }

    private:
        virtual ~item_features() {}

        mutable unsigned _ref_cnt;
        friend intrusive_ptr_add_ref(const item_features*);
        friend intrusive_ptr_release(const item_features*);
    }; 

    // Item interface

    virtual item_features::ptr_t add_item(const std::string & item_uid)
    { };

    virtual void drop_item(const std::string & item_uid)
    { };

    /// Estimator interface

    // Prepare for a sequence of calls to add_item_estimate
    virtual void reset_estimator();

    // train
    virtual void add_item_probability(
        const item_features::ptr_t, float prob_class_item);

    virtual void prepare_estimator();

    // decode prob(item | class)
    virtual float estimate_item(const item_features::ptr_t);
};

};

#endif
