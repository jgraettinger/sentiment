#include "features/sparse_features.hpp"
#include "conversions/vector.hpp"
#include <boost/python.hpp>
#include <algorithm>

// specializations allowing wrapping smart-pointer to const dense_features
namespace boost {

    template<class T>
    inline T* get_pointer( boost::intrusive_ptr<const T> const& p ){
        return const_cast< T* >( p.get() );
    }

    namespace python {

        template<class T>
        struct pointee< boost::intrusive_ptr<T const> >{
            typedef T type;
        };
    }
}

namespace features {
namespace bpl = boost::python;

bpl::dict sparse_features_as_dict(const sparse_features::ptr_t & s)
{
    bpl::dict d;

    for(sparse_features::const_iterator it = s->begin(); it != s->end(); ++it)
        d[it->first] = it->second;

    return d;
}

void make_sparse_features_bindings()
{
    bpl::class_<sparse_features,
        sparse_features::mutable_ptr_t, boost::noncopyable>(
        "SparseFeatures", bpl::init<const std::map<unsigned, double> & >())
        .def("inner_product", &sparse_features::inner_product)
        .def("normalize_L2", &sparse_features::normalize_L2)
        .def("as_dict", &sparse_features_as_dict);

    bpl::register_ptr_to_python< sparse_features::ptr_t>();

    // implicit conversion from mutable to non-mutable ptr
    bpl::implicitly_convertible<
        sparse_features::mutable_ptr_t, sparse_features::ptr_t>();

    // feature sequences
    conversions::vector_to_python< std::vector<sparse_features::ptr_t>
        >::register_conversion();
    conversions::vector_from_python< std::vector<sparse_features::ptr_t>
        >::register_conversion();
}
};

