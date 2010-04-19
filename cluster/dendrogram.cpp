
#include "cluster/dendrogram.hpp"
#include <boost/python.hpp>

namespace cluster {

void make_dendrogram_bindings()
{
    boost::python::class_<dendrogram, dendrogram::ptr_t, boost::noncopyable>(
        "dendrogram", boost::python::init<
            unsigned long long, dendrogram::ptr_t, dendrogram::ptr_t>())
    .def(boost::python::init<unsigned long long>())
    .add_property("item", &dendrogram::item)
    .add_property("size", &dendrogram::size)
    .add_property("left_child",  &dendrogram::left_child)
    .add_property("right_child", &dendrogram::right_child);
}

};

