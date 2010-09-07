#include <boost/python.hpp>

namespace features {
    void make_sparse_features_bindings();
    void make_dense_features_bindings();
};

BOOST_PYTHON_MODULE(_features)
{
    features::make_sparse_features_bindings();
    features::make_dense_features_bindings();
}
