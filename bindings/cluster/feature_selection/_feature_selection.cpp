
#include <boost/python.hpp>

namespace cluster {
namespace feature_selection {
    void make_information_gain_selector_bindings();
}
};

BOOST_PYTHON_MODULE(_feature_selection)
{
    cluster::feature_selection::make_information_gain_selector_bindings();
}
