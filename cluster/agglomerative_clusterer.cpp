
#include "cluster/group_avg_similarity.hpp"
#include "cluster/agglomerative_clusterer.hpp"
#include "util.hpp"
#include <list>


#include <iostream>

namespace cluster {
namespace bpl = boost::python;

void print_root(const std::list<
        std::pair<
            dendrogram::ptr_t,
            group_average_similarity::features_t
        >
    >::iterator & i)
{
    std::cout << "dendogram(" << i->first->size() << ") :";
    
    for(group_average_similarity::features_t::iterator it = i->second.begin();
        it != i->second.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << ", ";
    }
    std::cout << std::endl;
}

template<typename Similarity>
dendrogram::ptr_t agglomerative_clusterer<Similarity>::cluster(bpl::object items)
{
    typedef std::list<
        std::pair<
            dendrogram::ptr_t,
            typename Similarity::features_t
        >
    > clusters_t;

    clusters_t clusters;

    // item / feature extraction
    bpl::object iter = get_iterator(items);
    for(bpl::object item; next(iter, item);)
    {
        clusters.push_back( typename clusters_t::value_type(
            new dendrogram(bpl::extract<unsigned long long>(item[0])),
            sim.extract_features(item[1])
        ));
    }

    if(clusters.empty())
        return dendrogram::ptr_t();

    // core clustering loop
    while(clusters.size() > 1)
    {
        // find max-similarity root pair
        typename clusters_t::iterator it1, it2, max1, max2;
        float max_sim = -123456; 

        for(it1 = clusters.begin(); it1 != clusters.end(); ++it1)
        {
            for(it2 = it1; it2 != clusters.end(); ++it2)
            {
                if(it1 == it2)
                    continue;

                float cur_sim = sim.similarity(it1->second, it2->second,
                    it1->first->size(), it2->first->size());
                if(cur_sim > max_sim)
                {
                    max_sim = cur_sim;
                    max1 = it1;
                    max2 = it2;
                }
            }
        }

//        std::cout << "MERGE =================== " << max_sim << std::endl;
//        print_root(max1);
//        print_root(max2);

        if(max1->first->size() > max2->first->size())
            max1->first = dendrogram::ptr_t(
                new dendrogram(max1->first->item(), max1->first, max2->first));
        else
            max1->first = dendrogram::ptr_t(
                new dendrogram(max2->first->item(), max1->first, max2->first));

        max1->second = sim.merge_features(
            max1->second, max2->second);

        clusters.erase(max2);
        print_root(max1);
    }
    return clusters.begin()->first;
}

void make_agglomerative_clusterer_bindings()
{
    boost::python::class_<agglomerative_clusterer<group_average_similarity> >(
        "GAAClusterer", boost::python::init<>())
    .def("cluster", &agglomerative_clusterer<group_average_similarity>::cluster);
}

};

