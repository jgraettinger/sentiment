
#ifndef CLUSTER_DENDROGRAM_HPP
#define CLUSTER_DENDROGRAM_HPP

#include <boost/intrusive_ptr.hpp>

namespace cluster {

class dendrogram {
public:

    typedef boost::intrusive_ptr<dendrogram> ptr_t;

    dendrogram(
        unsigned long long item,
        const dendrogram::ptr_t & left_child,
        const dendrogram::ptr_t & right_child
    ) : _item(item),
        _size(left_child->size() + right_child->size()),
        _left_child(left_child),
        _right_child(right_child),
        _ref_cnt(0)
    { }

    dendrogram(unsigned long long item)
      : _item(item),
        _size(1),
        _ref_cnt(0)
    { }

    unsigned long long item()
    { return _item; }

    unsigned size()
    { return _size; }

    dendrogram::ptr_t left_child()
    { return _left_child; }

    dendrogram::ptr_t right_child()
    { return _right_child; }

private:

    unsigned long long _item;
    unsigned _size;
    ptr_t _left_child, _right_child;
    
    mutable unsigned _ref_cnt;

    friend void intrusive_ptr_add_ref(const dendrogram *);
    friend void intrusive_ptr_release(const dendrogram *);
};

inline void intrusive_ptr_add_ref(const dendrogram * p)
{ p->_ref_cnt += 1; }

inline void intrusive_ptr_release(const dendrogram * p)
{
    if(--(p->_ref_cnt) == 0)
    {
        delete (dendrogram*) p;
    }
}

}; // end namespace

#endif
