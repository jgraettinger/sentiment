#include <boost/python.hpp>

namespace cluster {
namespace feature_transform {
    void make_igain_cutoff_transform_bindings();
    void make_random_proj_transform_bindings();
};
};

BOOST_PYTHON_MODULE(_feature_transform)
{
    cluster::feature_transform::make_igain_cutoff_transform_bindings();
    cluster::feature_transform::make_random_proj_transform_bindings();
}
