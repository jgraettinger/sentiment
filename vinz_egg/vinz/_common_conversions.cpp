#include <boost/python.hpp>
#include "conversions/vector.hpp"
#include "conversions/map.hpp"
#include "conversions/pair.hpp"

#include <boost/unordered_map.hpp>
#include <vector>
#include <map>
#include <iostream>

using namespace conversions;

BOOST_PYTHON_MODULE(_common_conversions)
{
    vector_to_python< std::vector<double>
        >::register_conversion();
    vector_from_python< std::vector<double>
        >::register_conversion();

    vector_to_python< std::vector< std::vector<double> >
        >::register_conversion();
    vector_from_python< std::vector< std::vector<double> >
        >::register_conversion();

    map_to_python< std::map<unsigned, double>
        >::register_conversion();
    map_from_python< std::map<unsigned, double>
        >::register_conversion();

    map_to_python< boost::unordered_map<unsigned, double>
        >::register_conversion();
    map_from_python< boost::unordered_map<unsigned, double>
        >::register_conversion();

    // em_cluster::sample_cluster_state_t
    map_to_python< std::map<std::string, std::pair<double, bool> >
        >::register_conversion();
    map_from_python< std::map<std::string, std::pair<double, bool> >
        >::register_conversion();

    pair_to_python< std::pair<double, bool> >::register_conversion();
    pair_from_python< std::pair<double, bool> >::register_conversion();

    // feature_selection::selector_mixin
    vector_to_python< std::vector< std::pair<unsigned, double> >
        >::register_conversion();
    vector_from_python< std::vector< std::pair<unsigned, double> >
        >::register_conversion();

    pair_to_python< std::pair<unsigned, double> >::register_conversion();
    pair_from_python< std::pair<unsigned, double> >::register_conversion();
}

