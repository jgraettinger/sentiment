#include "cluster/featurizer.hpp"
#include "cluster/sample.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace bpl = boost::python;

void make_featurizer_bindings()
{
    bpl::class_<featurizer, featurizer::ptr_t, boost::noncopyable>(
        "featurizer", bpl::no_init)
    .def("featurize", bpl::pure_virtual(&featurizer::featurize));

    bpl::class_<cached_featurizer, cached_featurizer::ptr_t,
        bpl::bases<featurizer>, boost::noncopyable>(
        "cached_featurizer", bpl::init<const featurizer::ptr_t &>())
    .add_property("wrapped_featurizer",
        &cached_featurizer::get_wrapped_featurizer);
}

};

