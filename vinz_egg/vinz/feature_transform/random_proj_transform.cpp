
#include "feature_transform/transform_helper.hpp"
#include "feature_transform/random_projector_transform.hpp"
#include "features/sparse_features.hpp"
#include "features/dense_features.hpp"
#include <boost/python.hpp>

namespace feature_transform {
namespace bpl = boost::python;

void make_random_proj_transform_bindings()
{
    bpl::class_<random_projector_transform,
        random_projector_transform::ptr_t, boost::noncopyable>(
            "RandomProjTransform",
            bpl::init<unsigned>( bpl::args("n_output_features")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            random_projector_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            random_projector_transform, features::sparse_features>)
        .def("train_transform", &feature_transform::py_train_transform_helper<
            random_projector_transform, features::dense_features>)
        .def("transform", &feature_transform::py_transform_helper<
            random_projector_transform, features::dense_features>);
};

};

