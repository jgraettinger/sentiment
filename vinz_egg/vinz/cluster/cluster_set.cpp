
#include "cluster_set.hpp"
#include <boost/python.hpp>

namespace bpl = boost::python;

namespace cluster
{

void make_cluster_set_bindings()
{
    bpl::class_<cluster_set, cluster_set::ptr_t, boost::noncopyable>(
        "cluster_set", bpl::no_init)
    .add_property("uids", bpl::make_function(
        &cluster_set::get_uids, bpl::return_internal_reference<>()))
    .add_property("priors", &cluster_set::get_priors);
}

};

