
#include <boost/python.hpp>

namespace cluster {
namespace estimation {
    void make_naive_bayes_estimator_bindings();
}
};

BOOST_PYTHON_MODULE(_estimation)
{
    cluster::estimation::make_naive_bayes_estimator_bindings();
}

