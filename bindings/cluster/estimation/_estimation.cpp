
#include <boost/python.hpp>

namespace cluster {
namespace estimation {
    void make_naive_bayes_estimator_bindings();
    void make_gaussian_estimator_bindings();
}
};

BOOST_PYTHON_MODULE(_estimation)
{
    cluster::estimation::make_naive_bayes_estimator_bindings();
    cluster::estimation::make_gaussian_estimator_bindings();
}

