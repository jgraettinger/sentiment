
#include "cluster/group_avg_similarity.hpp"
#include "util.hpp"
#include <boost/python.hpp>
#include <cmath>

namespace cluster {

namespace bpl = boost::python;

group_average_similarity::features_t group_average_similarity::extract_features(
    boost::python::object o)
{
    features_t fout; fout.reserve(boost::python::len(o));

    float norm = 0;

    bpl::object iter = get_iterator(o);
    for(bpl::object feat; next(iter, feat);)
    {
        unsigned feat_id = bpl::extract<unsigned>(feat[0]);
        float feat_val = bpl::extract<float>(feat[1]);

        fout.push_back( features_t::value_type(
            feat_id, feat_val));
        norm += feat_val * feat_val;
    }

    // normalize to unit-length
    norm = 1.0 / sqrt(norm);
    for(features_t::iterator it = fout.begin(); it != fout.end(); ++it)
        it->second *= norm;

    std::sort(fout.begin(), fout.end());
    return fout;
}

};

