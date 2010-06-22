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
        "information_gain_selector", bpl::init<unsigned, double, unsigned>())
        .def("reset", &information_gain_selector::reset)
        .def("add_observation", &information_gain_selector::add_observation)
        .def("derive_statistics", &py_derive_statistics)
        .def("select_features", &information_gain_selector::select_features);
}

};
};

