#include "feature_transform/transform_helper.hpp"
#include "feature_transform/transforms.hpp"
#include "features/sparse_features.hpp"
#include <boost/python.hpp>

namespace feature_transform {
namespace bpl = boost::python;

template<typename StatisticProvider>
bpl::object py_derive_statistics(typename StatisticProvider::ptr_t stat_prov)
{
    double stat_sum = 0;
    typename StatisticProvider::feature_stat_t stats = \
        stat_prov->derive_statistics(stat_sum);

    bpl::dict d;
    for(unsigned i = 0; i != stats.size(); ++i)
        d[stats[i].first] = stats[i].second;

    return bpl::make_tuple(d, stat_sum);
}

void make_igain_cutoff_transform_bindings()
{
/*
    bpl::class_<information_gain_statistic,
        information_gain_statistic::ptr_t, boost::noncopyable>(
            "InformationGainStatistic",
            bpl::init<double>( bpl::args("class_smoothing_factor")))
        .def("reset", &information_gain_statistic::reset)
        .def("add_observation", &information_gain_statistic::add_observation<features::sparse_features>)
        .def("add_observation", &information_gain_statistic::add_observation<features::dense_features>)
        .def("derive_statistics", &py_derive_statistics<information_gain_statistic>);

    bpl::class_<igain_cutoff_transform,
        igain_cutoff_transform::ptr_t, boost::noncopyable>(
            "IGainCutoffTransform",
            bpl::init<unsigned, double, unsigned, const information_gain_statistic &>( bpl::args(
                "min_features", "max_mass_ratio", "max_features", "stat_provider")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            igain_cutoff_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            igain_cutoff_transform, features::sparse_features>);

    bpl::class_<proj_igain_cutoff_transform,
        proj_igain_cutoff_transform::ptr_t, boost::noncopyable>(
            "ProjIGainCutoffTransform",
            bpl::init<igain_cutoff_transform::ptr_t, random_projector_transform::ptr_t>( bpl::args(
                "igain_cutoff_transform", "random_proj_transform")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            proj_igain_cutoff_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            proj_igain_cutoff_transform, features::sparse_features>);

    bpl::class_<compact_igain_cutoff_transform,
        compact_igain_cutoff_transform::ptr_t, boost::noncopyable>(
            "CompactIGainCutoffTransform",
            bpl::init<igain_cutoff_transform::ptr_t, compact_transform::ptr_t>( bpl::args(
                "igain_cutoff_transform", "compact_transform")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            compact_igain_cutoff_transform, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            compact_igain_cutoff_transform, features::sparse_features>);
*/
}

};


