
inline size_t cluster_set::add_cluster(const std::string & uid)
{
    std::vector<std::string>::iterator c_it = lower_bound(
        _uids.begin(), _uids.end(), uid);

    if(c_it != _uids.end() && *c_it == uid)
        throw std::runtime_error("duplicate cluster UID " + uid);

    size_t ind = std::distance(_uids.begin(), c_it);

    _uids.insert(_uids.begin() + ind, uid); 

    priors.clear();
    return ind;
}

inline size_t cluster_set::drop_cluster(const std::string & uid)
{
    size_t ind = get_cluster_index(uid);

    _uids.erase(_uids.begin() + ind); 

    priors.clear();
    return ind;
}

inline size_t cluster_set::get_cluster_index(const std::string & uid)
{
    std::vector<std::string>::iterator c_it = lower_bound(
        _uids.begin(), _uids.end(), uid);

    if(c_it == _uids.end() && *c_it != uid)
        throw std::runtime_error("no cluster with UID " + uid);

    return std::distance(_uids.begin(), c_it);
}

inline boost::python::dict cluster_set::get_priors()
{
    if(priors.empty())
        throw std::runtime_error("cluster priors have not been calcluated;"
            " run an iteration of clustering");

    assert(priors.size() == _uids.size());

    boost::python::dict d;
    for(size_t i = 0; i != priors.size(); ++i)
    {
        d[_uids[i]] = priors[i];
    }
    return d;
}

