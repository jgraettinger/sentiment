
#include <boost/python.hpp>

void make_em_clusterer_bindings();

BOOST_PYTHON_MODULE(_vinz)
{
    make_em_clusterer_bindings();
}
