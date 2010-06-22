#ifndef CLUSTER_VECTOR_OPS_HPP
#define CLUSTER_VECTOR_OPS_HPP

#include <numeric>
#include <vector>

namespace cluster
{

class vector_ops
{
public:

    static void normalize_L1(std::vector<double> & v)
    {
        double t = 1.0 / std::accumulate(v.begin(), v.end(), 0);

        for(unsigned i = 0; i != v.size(); ++i)
            v[i] *= t;

        return;
    }

    static void normalize_L1(std::vector< std::pair<unsigned, double> > & v)
    {
        double t = 0;
        for(unsigned i = 0; i != v.size(); ++i)
            t += v[i].second;

        t = 1.0 / t;

        for(unsigned i = 0; i != v.size(); ++i)
            v[i].second *= t;

        return;
    }
};

};

#endif
