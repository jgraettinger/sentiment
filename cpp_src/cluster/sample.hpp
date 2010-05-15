
#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include "ref_counted.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

namespace cluster
{

// Sample interface

class sample : public ref_counted
{
public:

    typedef ref_counted::bind_ptr<sample>::ptr_t ptr_t;

    sample(const std::string & uid)
     : ref_counted(),
       _uid(uid)
    { };

    const std::string & get_uid() const
    { return _uid; }

protected:

    const std::string _uid;
};

class sample_features : public ref_counted
{
public:

    typedef bind_ptr<sample_features>::ptr_t ptr_t;
};

struct sample_uid_compare_functor
{
    inline bool operator()(const std::string & s1, const sample::ptr_t & s2) const
    { return s1 < s2->get_uid(); }

    inline bool operator()(const sample::ptr_t & s1, const std::string & s2) const
    { return s1->get_uid() < s2; }

    inline bool operator()(const sample::ptr_t & s1, const sample::ptr_t & s2) const
    { return s1->get_uid() < s2->get_uid(); }
};

// common sample types

class document_sample : public sample
{
public:

    typedef ref_counted::bind_ptr<document_sample>::ptr_t ptr_t;

    document_sample(
        const std::string & uid,
        const std::string & author,
        const std::string & content)
     : sample(uid),
       _author(author),
       _content(content)
    {}

    const std::string & get_author() const
    { return _author; }

    const std::string & get_content() const
    { return _content; }

protected:

    const std::string _author;
    const std::string _content;
};


};

#endif

