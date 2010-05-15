#include "cluster/vectorspace_features.hpp"
#include "cluster/featurizer.hpp"
#include "util.hpp"
#include <boost/python.hpp>

namespace cluster {
namespace bpl = boost::python;

class py_sparse_vectorspace_featurizer :
    public featurizer, public bpl::wrapper<featurizer>
{
public:

    typedef boost::shared_ptr<py_sparse_vectorspace_featurizer> ptr_t; 

    sample_features::ptr_t featurize(const sample::ptr_t & s)
    {
        sparse_vectorspace_features::ptr_t feat_ptr(
            new sparse_vectorspace_features());

        sparse_vectorspace_features & feat(*feat_ptr);

        // call subclass featurize()
        bpl::object o = this->get_override("featurize")(s);

        feat.reserve(bpl::len(o));

        double norm = 0;
        for(bpl::object f, iter = get_iterator(o); next(iter, f);)
        {
            unsigned feat_id = bpl::extract<unsigned>(f[0]);
            double feat_val = bpl::extract<double>(f[1]);

            feat.push_back( std::make_pair(feat_id, feat_val));
            norm += feat_val * feat_val;
        }

        // normalize to unit-length
        norm = 1.0 / std::sqrt(norm);
        for(unsigned i = 0; i != feat.size(); ++i)
            feat[i].second *= norm;

        std::sort(feat.begin(), feat.end());
        return feat_ptr;
    };
};

class py_dense_vectorspace_featurizer :
    public featurizer, public bpl::wrapper<featurizer>
{
public:

    typedef boost::shared_ptr<py_dense_vectorspace_featurizer> ptr_t; 

    sample_features::ptr_t featurize(const sample::ptr_t & s)
    {
        dense_vectorspace_features::ptr_t feat_ptr(
            new dense_vectorspace_features());
        dense_vectorspace_features & feat(*feat_ptr);

        // call subclass featurize()
        bpl::object o(this->get_override("featurize")(s));
        feat.reserve(bpl::len(o));

        double norm = 0;
        for(bpl::object f, iter = get_iterator(o); next(iter, f);)
        {
            double feat_val = bpl::extract<double>(f);

            feat.push_back(feat_val);
            norm += feat_val * feat_val;
        }

        // normalize to unit-length
        norm = 1.0 / std::sqrt(norm);
        for(unsigned i = 0; i != feat.size(); ++i)
            feat[i] *= norm;

        return feat_ptr;
    };
};

void make_vectorspace_features_bindings()
{
    bpl::class_<py_sparse_vectorspace_featurizer,
        py_sparse_vectorspace_featurizer::ptr_t,
        boost::noncopyable>(
        "sparse_vectorspace_featurizer", bpl::init<>())
    .def("featurize", bpl::pure_virtual(&featurizer::featurize));

    bpl::class_<py_dense_vectorspace_featurizer,
        py_dense_vectorspace_featurizer::ptr_t,
        boost::noncopyable>(
        "dense_vectorspace_featurizer", bpl::init<>())
    .def("featurize", bpl::pure_virtual(&featurizer::featurize));
}

};
