
#include "cluster/feature_transform/transform_helper.hpp"
#include "cluster/feature_transform/transforms.hpp"
#include "cluster/features/sparse_features.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace feature_transform {
namespace bpl = boost::python;

void make_pca_proj_transform_bindings()
{
    bpl::class_<pca_projector_transform,
        pca_projector_transform::ptr_t, boost::noncopyable>(
            "PCAProjTransform",
            bpl::init<unsigned>( bpl::args("n_output_features")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            pca_projector_transform, features::dense_features>)
        .def("transform", &feature_transform::py_transform_helper<
            pca_projector_transform, features::dense_features>)
        .def("train_transform", &feature_transform::py_train_transform_helper<
            pca_projector_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            pca_projector_transform, features::sparse_features>)
        .def("get_eigenvalue", &pca_projector_transform::get_eigenvalue)
        .def("get_eigenvector", &pca_projector_transform::get_eigenvector);

    bpl::class_<pca_proj_igain_cutoff_transform,
        pca_proj_igain_cutoff_transform::ptr_t, boost::noncopyable>(
            "PCAProjIGainCutoffTransform",
            bpl::init<proj_igain_cutoff_transform::ptr_t,
                      pca_projector_transform::ptr_t>( bpl::args(
                "proj_igain_cutoff_transform", "pca_proj_transform")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            pca_proj_igain_cutoff_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            pca_proj_igain_cutoff_transform, features::sparse_features>);

    bpl::class_<pca_compact_igain_cutoff_transform,
        pca_compact_igain_cutoff_transform::ptr_t, boost::noncopyable>(
            "PCACompactIGainCutoffTransform",
            bpl::init<compact_igain_cutoff_transform::ptr_t,
                      pca_projector_transform::ptr_t>( bpl::args(
                "compact_igain_cutoff_transform", "pca_proj_transform")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            pca_compact_igain_cutoff_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            pca_compact_igain_cutoff_transform, features::sparse_features>);

    bpl::class_<pca_igain_cutoff_transform,
        pca_igain_cutoff_transform::ptr_t, boost::noncopyable>(
            "PCAIGainCutoffTransform",
            bpl::init<igain_cutoff_transform::ptr_t,
                      pca_projector_transform::ptr_t>( bpl::args(
                "igain_cutoff_transform", "pca_proj_transform")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            pca_igain_cutoff_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            pca_igain_cutoff_transform, features::sparse_features>);
}

};
};

