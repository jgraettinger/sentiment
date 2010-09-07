
#include "feature_transform/transforms.hpp"
#include "em_clusterer.hpp"
#include "features/sparse_features.hpp"
#include "features/dense_features.hpp"
#include "estimation/gaussian_estimator.hpp"

#include <boost/python.hpp>

namespace bpl = boost::python;

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
/*    bind_em_clusterer<
        estimation::naive_bayes_estimator,
        feature_selection::information_gain_selector
    >("NaiveBayesEmClusterer");

    typedef em_clusterer<
        features::dense_features,
        estimation::gaussian_estimator> est_t;

    bind_em_clusterer< em_clusterer<
        features::dense_features,
        estimation::gaussian_estimator>
    >("DenseGaussEmClusterer")
    .def("transform_features", &est_t::transform_features<
        feature_transform::random_projector_transform>)
    .def("transform_features", &est_t::transform_features<
        feature_transform::chained_transform<
            feature_transform::random_projector_transform,
            feature_transform::random_projector_transform> >);
*/

    typedef em_clusterer<
        features::sparse_features,
        estimation::gaussian_estimator> est2_t;

    bind_em_clusterer< em_clusterer<
        features::sparse_features,
        estimation::gaussian_estimator>
    >("SparseGaussEmClusterer")
    .def("transform_features", &est2_t::transform_features<
        feature_transform::proj_igain_cutoff_transform> );
}

