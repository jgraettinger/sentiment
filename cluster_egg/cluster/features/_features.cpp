#include <boost/python.hpp>

namespace cluster {
namespace features {
    void make_sparse_features_bindings();
    void make_dense_features_bindings();
};
};

BOOST_PYTHON_MODULE(_features)
{
    cluster::features::make_sparse_features_bindings();
    cluster::features::make_dense_features_bindings();
}
