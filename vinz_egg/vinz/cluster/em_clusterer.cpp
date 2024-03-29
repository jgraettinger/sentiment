
#include "feature_transform/transforms.hpp"
#include "cluster/em_clusterer.hpp"
#include "features/sparse_features.hpp"
#include "features/dense_features.hpp"
#include "estimation/gaussian_estimator.hpp"

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
    .def("add_sample", &Clusterer::add_sample)
    .def("drop_sample", &Clusterer::drop_sample)
    .def("get_sample_probabilities", &Clusterer::get_sample_probabilities)
    .def("get_estimator_features", &Clusterer::get_estimator_features)
    .def("expect_and_maximize", &Clusterer::expect_and_maximize)
    .def("get_sample_likelihood", &Clusterer::get_sample_likelihood);
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
        feature_transform::pca_projector_transform>)
    .def("transform_features", &sparse_clusterer_t::transform_features<
        feature_transform::proj_igain_cutoff_transform>);
}

};

