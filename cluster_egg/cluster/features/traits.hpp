#ifndef CLUSTER_FEATURE_TRAITS_HPP
#define CLUSTER_FEATURE_TRAITS_HPP

namespace cluster
{
namespace features
{

template<typename Features>
struct reference
{ typedef const typename Features::ptr_t & type; };

template<typename Features>
struct value
{ typedef typename Features::ptr_t type; };

template<typename Features>
struct iterator
{
    typedef typename Features::const_iterator type;

    static type begin(typename reference<Features>::type f)
    { return f->begin(); }

    static type end(typename reference<Features>::type f)
    { return f->end(); }
};

// helpers to avoid explict specification of
// iterator struct in getting begin/end iterators
template<typename Features>
inline typename iterator<Features>::type begin(
    typename reference<Features>::type f)
{
    return iterator<Features>::begin(f);
}

template<typename Features>
inline typename iterator<Features>::type end(
    typename reference<Features>::type f)
{
    return iterator<Features>::end(f);
}

template<typename FeaturesIterator>
inline unsigned deref_id(const FeaturesIterator & it)
{ return it->first; }

template<typename FeaturesIterator>
inline double deref_value(const FeaturesIterator & it)
{ return it->second; }

};
};

#endif
