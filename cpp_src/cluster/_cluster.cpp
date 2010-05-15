
#include <boost/python.hpp>

namespace cluster {

    void make_em_cluster_bindings();
    void make_estimator_bindings();
    void make_featurizer_bindings();
    void make_sample_bindings();
    void make_vectorspace_estimator_bindings();
    void make_vectorspace_features_bindings();

};

BOOST_PYTHON_MODULE(_cluster)
{
    cluster::make_em_cluster_bindings();
    cluster::make_estimator_bindings();
    cluster::make_featurizer_bindings();
    cluster::make_sample_bindings();
    cluster::make_vectorspace_estimator_bindings();
    cluster::make_vectorspace_features_bindings();
}
