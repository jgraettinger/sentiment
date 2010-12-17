#include <boost/python.hpp>

namespace feature_transform {
    void make_idf_transform_bindings();
    void make_pca_transform_bindings();
    void make_idf_pca_transform_bindings();
    void make_random_proj_transform_bindings();

/*
    void make_igain_cutoff_transform_bindings();
    void make_compact_transform_bindings();
    void make_idf_transform_bindings();
*/
};

BOOST_PYTHON_MODULE(_feature_transform)
{
    feature_transform::make_idf_transform_bindings();
    feature_transform::make_pca_transform_bindings();
    feature_transform::make_idf_pca_transform_bindings();
    feature_transform::make_random_proj_transform_bindings();
 
/*
    feature_transform::make_igain_cutoff_transform_bindings();
    feature_transform::make_pca_proj_transform_bindings();
    feature_transform::make_compact_transform_bindings();
    feature_transform::make_idf_transform_bindings();
*/
}
