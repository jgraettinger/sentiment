#include "cluster/sample.hpp"
#include <boost/python.hpp>
#include <string>

namespace cluster {
namespace bpl = boost::python;

void make_sample_bindings()
{
    bpl::class_<sample, sample::ptr_t,
        boost::noncopyable>("sample", bpl::no_init)
    .add_property("uid",
        bpl::make_function(&sample::get_uid,
            bpl::return_value_policy<bpl::copy_const_reference>()));

    bpl::class_<sample_features, sample_features::ptr_t,
        boost::noncopyable>("sample_features", bpl::no_init);

    bpl::class_<document_sample, document_sample::ptr_t, bpl::bases<sample>,
        boost::noncopyable >("document_sample", bpl::init<
            const std::string &, const std::string &, const std::string &>())
    .add_property("author",
        bpl::make_function(&document_sample::get_author,
            bpl::return_value_policy<bpl::copy_const_reference>()))
    .add_property("content",
        bpl::make_function(&document_sample::get_content,
            bpl::return_value_policy<bpl::copy_const_reference>()));

    // mark intrusive_ptr<Derived> as convertible to intrusive_ptr<Base>
    bpl::implicitly_convertible<document_sample::ptr_t, sample::ptr_t>();
}

};

