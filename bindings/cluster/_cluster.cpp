
#include <boost/python.hpp>

namespace cluster {
    void make_em_clusterer_bindings();
    void make_sparse_features_bindings();
    void make_dense_features_bindings();
};

BOOST_PYTHON_MODULE(_cluster)
{
    cluster::make_em_clusterer_bindings();
    cluster::make_sparse_features_bindings();
    cluster::make_dense_features_bindings();
}
