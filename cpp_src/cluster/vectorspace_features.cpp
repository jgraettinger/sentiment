#include "cluster/vectorspace_features.hpp"
#include "cluster/featurizer.hpp"
#include "util.hpp"
#include <boost/python.hpp>

#include <iostream>

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
        bpl::object o = this->get_override("_py_featurize")(s);

        feat.reserve(bpl::len(o));

        for(bpl::object f, iter = get_iterator(o); next(iter, f);)
        {
            unsigned feat_id = bpl::extract<unsigned>(f[0]);
            double feat_val = bpl::extract<double>(f[1]);

            feat.push_back( std::make_pair(feat_id, feat_val));
        }

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
        bpl::object o = this->get_override("_py_featurize")(s);
        feat.reserve(bpl::len(o));

        for(bpl::object f, iter = get_iterator(o); next(iter, f);)
        {
            double feat_val = bpl::extract<double>(f);

            feat.push_back(feat_val);
        }

        return feat_ptr;
    };
};

void make_vectorspace_features_bindings()
{
    bpl::class_<py_sparse_vectorspace_featurizer,
        py_sparse_vectorspace_featurizer::ptr_t,
        boost::noncopyable>(
        "sparse_vectorspace_featurizer", bpl::init<>())
    .def("featurize", &featurizer::featurize);

    bpl::class_<py_dense_vectorspace_featurizer,
        py_dense_vectorspace_featurizer::ptr_t,
        boost::noncopyable>(
        "dense_vectorspace_featurizer", bpl::init<>())
    .def("featurize", &featurizer::featurize);
}

};
