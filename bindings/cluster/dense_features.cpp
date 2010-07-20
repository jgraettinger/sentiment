#include "cluster/dense_features.hpp"
#include "conversions/vector.hpp"
#include <boost/python.hpp>
#include <algorithm>

// specializations allowing wrapping smart-pointer to const dense_features
namespace boost {
    using namespace cluster;

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

namespace boost{ namespace python{


} } //boost::python

namespace cluster {
namespace bpl = boost::python;

bpl::list dense_features_as_list(const dense_features::ptr_t & s)
{
    bpl::list l;

    for(dense_features::const_iterator it = s->begin(); it != s->end(); ++it)
        l.append(*it);

    return l;
}

void make_dense_features_bindings()
{
    bpl::class_<dense_features, dense_features::mutable_ptr_t, boost::noncopyable>(
        "DenseFeatures", bpl::init<unsigned>())
        .def( bpl::init<const std::vector<double> &>())
        .def("as_list", &dense_features_as_list);
    
    bpl::register_ptr_to_python< dense_features::ptr_t>();

    // implicit conversion from mutable to non-mutable ptr
    bpl::implicitly_convertible<
        dense_features::mutable_ptr_t, dense_features::ptr_t>();
}

};
