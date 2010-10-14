
#include <boost/python.hpp>

namespace cluster {
void make_em_clusterer_bindings();
void make_cluster_set_bindings();
void make_cluster_sample_bindings();
};

BOOST_PYTHON_MODULE(_cluster)
{
    cluster::make_em_clusterer_bindings();
    cluster::make_cluster_set_bindings();
    cluster::make_cluster_sample_bindings();
}
