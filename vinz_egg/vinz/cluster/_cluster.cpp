
#include <boost/python.hpp>

namespace cluster {
void make_em_clusterer_bindings();
};

BOOST_PYTHON_MODULE(_cluster)
{
    cluster::make_em_clusterer_bindings();
}
