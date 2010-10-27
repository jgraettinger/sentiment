
#include "feature_transform/transform_helper.hpp"
#include "feature_transform/idf_transform.hpp"
#include "features/sparse_features.hpp"
#include "features/dense_features.hpp"
#include <boost/python.hpp>

namespace feature_transform {
namespace bpl = boost::python;

void make_idf_transform_bindings()
{
    bpl::class_<idf_transform,
        idf_transform::ptr_t, boost::noncopyable>("IdfTransform",
            bpl::init<double, unsigned>(bpl::args("max_df_threshold", "min_df_count")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            idf_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            idf_transform, features::sparse_features>)
        .def("train_transform", &feature_transform::py_train_transform_helper<
            idf_transform, features::dense_features>)
        .def("transform", &feature_transform::py_transform_helper<
            idf_transform, features::dense_features>);
};

};

