
namespace cluster {

class fuzzy_cmeeans
{
    /*
     * Inputs
     * @arg clusters: [cluster-UID]
     * @arg documents: iterable of
     *        (document-UID, (hard_clusters, soft_clusters, features))
     *
     * hard_clusters: {cluster-UID: [0,1]}
     * soft_clusters: {cluster-UID: [0,1]}
     * features:      features_t 
     */
    void cluster(boost::python::object clusters, boost::python::object documents);

};

};

