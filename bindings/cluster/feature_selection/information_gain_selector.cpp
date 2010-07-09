#include "cluster/feature_selection/information_gain_selector.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace feature_selection {
namespace bpl = boost::python;

bpl::object py_derive_statistics(information_gain_selector::ptr_t ig)
{
    double stat_sum = 0;
    information_gain_selector::feature_stat_t stats = \
        ig->derive_statistics(stat_sum);

    bpl::dict d;
    for(unsigned i = 0; i != stats.size(); ++i)
        d[stats[i].first] = stats[i].second;

    return bpl::make_tuple(d, stat_sum);
}

void make_information_gain_selector_bindings()
{
    bpl::class_<information_gain_selector,
        information_gain_selector::ptr_t, boost::noncopyable>(
        "InformationGainSelector",
        bpl::init<unsigned, double, unsigned, double>(
            bpl::args("min_features", "max_mass_ratio",
                "max_features", "class_smoothing_factor")))
        .def("reset", &information_gain_selector::reset)
        .def("add_observation", &information_gain_selector::add_observation)
        .def("derive_statistics", &py_derive_statistics)
        .def("select_features", &information_gain_selector::select_features)
        .def("prepare_selector", &information_gain_selector::prepare_selector)
        .def("filter_features", &information_gain_selector::filter_features)
        .def("filter_and_normalize_features",
            &information_gain_selector::filter_and_normalize_features);
}

};
};

