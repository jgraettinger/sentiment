
#include "feature_transform/transform_helper.hpp"
#include "feature_transform/transforms.hpp"
#include "features/sparse_features.hpp"
#include <boost/python.hpp>

namespace feature_transform {
namespace bpl = boost::python;

void make_idf_pca_transform_bindings()
{
    typedef chained_transform<
        idf_transform,
        pca_transform
    > transform_t;

    bpl::class_<transform_t,
        transform_t::ptr_t, boost::noncopyable>("IdfPcaTransform", bpl::init<
            idf_transform::ptr_t, pca_transform::ptr_t>(
                bpl::args("idf_transform", "pca_transform")))
        .def("train_transform", &feature_transform::py_train_transform_helper<
            transform_t, features::sparse_features>)
        .def("transform", &feature_transform::py_transform_helper<
            transform_t, features::sparse_features>);
}

};

