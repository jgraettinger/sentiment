#include "cluster/feature_selection/information_gain_selector.hpp"
#include "cluster/feature_selection/selector_mixin.hpp"
#include "cluster/feature_selection/filter_mixin.hpp"
#include "cluster/feature_selection/random_projector.hpp"
#include "cluster/sparse_features.hpp"
#include "cluster/dense_features.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace feature_selection {
namespace bpl = boost::python;

class information_gain_filter;
class information_gain_filter :
    public information_gain_mixin<information_gain_filter, sparse_features>,
    public selector_mixin<information_gain_filter>,
    public filter_mixin<information_gain_filter, sparse_features>
{
public:

    typedef boost::shared_ptr<information_gain_filter> ptr_t;
    typedef std::vector<std::pair<unsigned, double> > feature_stat_t;

    information_gain_filter(
        double class_smoothing_factor,
        unsigned min_features,
        double max_mass_ratio,
        unsigned max_features
    )
     : information_gain_mixin<
        information_gain_filter, sparse_features
       >(class_smoothing_factor),
       selector_mixin<information_gain_filter>(
        min_features,
        max_mass_ratio,
        max_features)
    { }
};

class information_gain_projector;
class information_gain_projector :
    public information_gain_mixin<information_gain_projector, sparse_features>,
    public selector_mixin<information_gain_projector>,
    public random_projector_mixin<information_gain_projector, sparse_features, dense_features>
{
public:

    typedef boost::shared_ptr<information_gain_projector> ptr_t;
    typedef std::vector<std::pair<unsigned, double> > feature_stat_t;

    information_gain_projector(
        double class_smoothing_factor,
        unsigned min_features,
        double max_mass_ratio,
        unsigned max_features,
        unsigned n_output_features
    )
     : information_gain_mixin<
        information_gain_projector, sparse_features
       >(class_smoothing_factor),
       selector_mixin<information_gain_projector>(
        min_features,
        max_mass_ratio,
        max_features),
       random_projector_mixin<
        information_gain_projector, sparse_features, dense_features
       >(n_output_features)
    { }
};


template<typename IGain>
bpl::object py_derive_statistics(typename IGain::ptr_t ig)
{
    double stat_sum = 0;
    typename IGain::feature_stat_t stats = \
        ig->derive_statistics(stat_sum);

    bpl::dict d;
    for(unsigned i = 0; i != stats.size(); ++i)
        d[stats[i].first] = stats[i].second;

    return bpl::make_tuple(d, stat_sum);
}

void make_information_gain_selector_bindings()
{
    bpl::class_<information_gain_filter,
        information_gain_filter::ptr_t, boost::noncopyable>(
        "InformationGainFilter",
        bpl::init<double, unsigned, double, unsigned>(
            bpl::args("class_smoothing_factor",
            "min_features", "max_mass_ratio", "max_features")))
        .def("reset", &information_gain_filter::reset)
        .def("add_observation", &information_gain_filter::add_observation)
        .def("derive_statistics", &py_derive_statistics<information_gain_filter>)
        .def("select_features", &information_gain_filter::select_features)
        .def("prepare_selector", &information_gain_filter::prepare_selector)
        .def("filter_features", &information_gain_filter::filter_features);
    
    bpl::class_<information_gain_projector,
        information_gain_projector::ptr_t, boost::noncopyable>(
        "InformationGainProjector",
        bpl::init<double, unsigned, double, unsigned, unsigned>(
            bpl::args("class_smoothing_factor",
            "min_features", "max_mass_ratio", "max_features",
            "n_output_features")))
        .def("reset", &information_gain_projector::reset)
        .def("add_observation", &information_gain_projector::add_observation)
        .def("derive_statistics", &py_derive_statistics<information_gain_projector>)
        .def("select_features", &information_gain_projector::select_features)
        .def("prepare_selector", &information_gain_projector::prepare_selector)
        .def("filter_features", &information_gain_projector::filter_features);
}

};
};

