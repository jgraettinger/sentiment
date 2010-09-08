
#include <boost/python.hpp>

namespace estimation {
//    void make_naive_bayes_estimator_bindings();
    void make_gaussian_estimator_bindings();
};

BOOST_PYTHON_MODULE(_estimation)
{
//    estimation::make_naive_bayes_estimator_bindings();
    estimation::make_gaussian_estimator_bindings();
}

