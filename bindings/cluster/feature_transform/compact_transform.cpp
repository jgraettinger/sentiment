
#include "cluster/feature_transform/transform_helper.hpp"
#include "cluster/feature_transform/compact_transform.hpp"
#include "cluster/features/sparse_features.hpp"
#include "cluster/features/dense_features.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace feature_transform {
namespace bpl = boost::python;

void make_compact_transform_bindings()
{
    bpl::class_<compact_transform,
        compact_transform::ptr_t, boost::noncopyable>(
            "CompactTransform", bpl::init<>())
        .def("train_transform", &feature_transform::py_train_transform_helper<
            compact_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            compact_transform, features::sparse_features>)
        .def("train_transform", &feature_transform::py_train_transform_helper<
            compact_transform, features::dense_features>)
        .def("transform", &feature_transform::py_transform_helper<
            compact_transform, features::dense_features>);
};

};
};

