#include "cluster/features/sparse_features.hpp"
#include "conversions/map.hpp"
#include <boost/python.hpp>
#include <algorithm>

namespace cluster {
namespace features {
namespace bpl = boost::python;

bpl::dict sparse_features_as_dict(const sparse_features::ptr_t & s)
{
    bpl::dict d;

    for(sparse_features::const_iterator it = s->begin(); it != s->end(); ++it)
        d[it->first] = it->second;

    return d;
}

void make_sparse_features_bindings()
{
    bpl::class_<sparse_features, sparse_features::mutable_ptr_t, boost::noncopyable>(
        "SparseFeatures", bpl::init<const std::map<unsigned, double> & >())
        .def("as_dict", &sparse_features_as_dict);

    // implicit conversion from mutable to non-mutable ptr
    bpl::implicitly_convertible<
        sparse_features::mutable_ptr_t, sparse_features::ptr_t>();
}
};
};
