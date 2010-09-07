
#include "feature_transform/transform_helper.hpp"
#include "feature_transform/compact_transform.hpp"
#include "features/sparse_features.hpp"
#include "features/dense_features.hpp"
#include <boost/python.hpp>

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

