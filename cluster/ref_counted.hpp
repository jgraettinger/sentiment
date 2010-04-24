#ifndef REF_COUNTED_HPP
#define REF_COUNTED_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>

class ref_counted : private boost::noncopyable
{
public:

    template<typename Kls>
    struct bind_ptr {
        typedef boost::intrusive_ptr<Kls> ptr_t;
    };

    ref_counted()
     : _ref_cnt(0)
    { }

protected:

    virtual ~ref_counted()
    { }

private:

    mutable unsigned _ref_cnt;
    friend void intrusive_ptr_add_ref(const ref_counted*);
    friend void intrusive_ptr_release(const ref_counted*);
};

inline void intrusive_ptr_add_ref(const ref_counted * p)
{
    ++p->_ref_cnt;
}
inline void intrusive_ptr_release(const ref_counted * p)
{
    if(--p->_ref_cnt == 0)
    {
        delete (ref_counted *)(p);
    }
}

#endif

