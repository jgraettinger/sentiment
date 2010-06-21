#include "cluster/information_gain_selector.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace bpl = boost::python;

void make_information_gain_selector_bindings()
{
    bpl::class_<information_gain_selector,
        information_gain_selector::ptr_t, boost::noncopyable>(
        "information_gain_selector", bpl::init<>())
        .def("reset", &information_gain_selector::reset)
        .def("add_observation", &information_gain_selector::add_observation)
        .def("get_information_gain", &information_gain_selector::get_information_gain);
}

};

