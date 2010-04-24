#ifndef UTIL_HPP
#define UTIL_HPP

#include <boost/python.hpp>

/*
#include <cmath>

struct normalize {
    
    typedef std::vector<unsigned, float> fvec_t;
    
    float operator ()(fvec_t & v) const
    {
        float n = 0;
        for(fvec_t::iterator it = v.begin(); it != v.end(); ++it)
            n += it->second * it->second;
        
        n = 1 / std::sqrt<float>(n);
        
        for(fvec_t::iterator it = v.begin(); it != v.end(); ++it)
            it->second *= n;
        
        return;
    }
};
*/

// Methods for working with python sequences
inline boost::python::object get_iterator(boost::python::object o)
{
    // if dict, call o.iteritems()
    if(boost::python::extract<boost::python::dict>(o).check())
        return boost::python::extract<boost::python::dict>(o)().iteritems();
    
    // call iter(o) {returns new reference}
    return boost::python::object((boost::python::detail::new_reference)
        PyObject_GetIter(o.ptr()));
}

inline bool next(const boost::python::object & iter, boost::python::object & o)
{
    PyObject * py_ptr = PyIter_Next(iter.ptr());
    
    if(!py_ptr)
        return false;
    
    o = boost::python::object((boost::python::detail::new_reference) py_ptr);
    return true;
}

#endif
