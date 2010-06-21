#ifndef REF_COUNTED_HPP
#define REF_COUNTED_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>

template<typename Derived>
class static_ref_counted;

template<typename Derived>
inline void intrusive_ptr_add_ref(
    const static_ref_counted<Derived> * p)
{
    ++p->_ref_cnt;
}

template<typename Derived>
inline void intrusive_ptr_release(
    const static_ref_counted<Derived> * p)
{
    if(--p->_ref_cnt == 0)
    {
        // call derived destructor
        delete (Derived *)(p);
    }
}

// Reference-counted class intended for compile-time
//   polymorphism; no virtual destructor!
template<typename Derived>
class static_ref_counted :
    private boost::noncopyable
{
public:

    template<typename Kls>
    struct bind_ptr {
        typedef boost::intrusive_ptr<Kls> ptr_t;
    };

    static_ref_counted()
     : _ref_cnt(0)
    { }

private:

    mutable unsigned _ref_cnt;

    friend void intrusive_ptr_add_ref<Derived>(
        const static_ref_counted<Derived> *);
    friend void intrusive_ptr_release<Derived>(
        const static_ref_counted<Derived> *);
};

// Reference-counted class suitable
//   for run-time polymorphism
class ref_counted :
    public static_ref_counted<ref_counted>
{
protected:

    // can be safely derived from
    virtual ~ref_counted()
    { }
};


#endif

