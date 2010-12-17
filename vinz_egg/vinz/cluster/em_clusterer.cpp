
#include "feature_transform/transforms.hpp"
#include "cluster/em_clusterer.hpp"
#include "features/sparse_features.hpp"
#include "features/dense_features.hpp"
#include "estimation/gaussian_estimator.hpp"
#include "estimation/inner_product_estimator.hpp"

#include <boost/python.hpp>

namespace bpl = boost::python;

namespace cluster
{

template<typename Clusterer>
bpl::class_<Clusterer> bind_em_clusterer(const char * name)
{
    return bpl::class_<Clusterer>(name, bpl::init<>())
    .def("add_cluster", &Clusterer::add_cluster)
    .def("drop_cluster", &Clusterer::drop_cluster)
    .add_property("cluster_set", &Clusterer::get_cluster_set)
    .def("add_sample", &Clusterer::add_sample)
    .def("get_sample", &Clusterer::get_sample)
    .add_property("samples", &Clusterer::py_samples)
    .def("drop_sample", &Clusterer::drop_sample)
    .def("expect_and_maximize", &Clusterer::expect_and_maximize);
}

void make_em_clusterer_bindings()
{
    typedef em_clusterer<
        features::dense_features,
        estimation::gaussian_estimator> dense_clusterer_t;

    bind_em_clusterer<dense_clusterer_t>("DenseGaussEmClusterer");

    typedef em_clusterer<
        features::sparse_features,
        estimation::gaussian_estimator> sparse_clusterer_t;

    bind_em_clusterer<sparse_clusterer_t>("SparseGaussEmClusterer")
    .def("transform_features", &sparse_clusterer_t::transform_features<
        feature_transform::pca_transform>)
    .def("transform_features", &sparse_clusterer_t::transform_features<
        feature_transform::random_projector_transform>)
    .def("transform_features", &sparse_clusterer_t::transform_features<
        feature_transform::idf_pca_transform>);
/*    .def("transform_features", &sparse_clusterer_t::transform_features<
        feature_transform::pca_igain_cutoff_transform>)
    .def("transform_features", &sparse_clusterer_t::transform_features<
        feature_transform::proj_igain_cutoff_transform>);
*/

    typedef em_clusterer<
        features::sparse_features,
        estimation::inner_product_estimator> inner_product_clusterer_t;

    bind_em_clusterer<inner_product_clusterer_t>("InnerProductClusterer")
    .def("transform_features", &inner_product_clusterer_t::transform_features<
        feature_transform::idf_transform>);
}

};

