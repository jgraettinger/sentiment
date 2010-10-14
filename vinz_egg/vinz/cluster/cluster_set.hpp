#ifndef VINZ_CLUSTER_FWD_HPP
#define VINZ_CLUSTER_FWD_HPP

#include "ref_counted.hpp"
#include <boost/python.hpp>
#include <vector>

namespace cluster
{

struct cluster_set :
    public static_ref_counted<cluster_set>
{
public:

    typedef bind_ptr<cluster_set>::ptr_t ptr_t;

    // returns index inserted into, or throws
    size_t add_cluster(const std::string & uid);

    // returns index deleted from, or throws
    size_t drop_cluster(const std::string & uid);

    // returns index, or throws
    size_t get_cluster_index(const std::string & uid);

    const std::vector<std::string> & get_uids()
    { return _uids; }

    size_t size() const
    { return _uids.size(); }

    boost::python::dict get_priors();

    std::vector<double>    priors;

private:

    std::vector<std::string> _uids;
};

#include "cluster_set.impl.hpp"

};

#endif

