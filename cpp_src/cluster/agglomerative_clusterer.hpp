
#ifndef CLUSTER_AGGLOMERATIVE_CLUSTERER_HPP
#define CLUSTER_AGGLOMERATIVE_CLUSTERER_HPP

#include "cluster/dendrogram.hpp"
#include <boost/python.hpp>
#include <vector>

namespace cluster {

template<typename Similarity>
class agglomerative_clusterer {
public:
    
    dendrogram::ptr_t cluster(boost::python::object items);

private:

    Similarity sim;
};

};

#endif

