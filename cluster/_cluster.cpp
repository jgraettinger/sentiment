
#include <boost/python.hpp>

namespace cluster {

void make_dendrogram_bindings();
void make_agglomerative_clusterer_bindings();

};

BOOST_PYTHON_MODULE(_cluster)
{
    cluster::make_dendrogram_bindings();
    cluster::make_agglomerative_clusterer_bindings();
}
