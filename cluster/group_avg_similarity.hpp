
#ifndef CLUSTER_GROUP_AVG_SIMILARITY_HPP
#define CLUSTER_GROUP_AVG_SIMILARITY_HPP

#include <boost/python.hpp>
#include <vector>

namespace cluster {

struct group_average_similarity {
    
    // http://nlp.stanford.edu/IR-book/html/htmledition/
    //      group-average-agglomerative-clustering-1.html
    
    typedef std::vector< std::pair<unsigned, float> > features_t;
    
    float similarity(
        const features_t & c1, const features_t & c2,
        float c1_count, float c2_count
    ) const
    {
        /*
         * Computes & returns similarity of two sized clusters C1 & C2,
         * where similarity is defined as the average similarity
         * of item pairs i,j {elem union(C1,C2)} s.t. i != j.
         *
         * For efficiency, similarity is computed as the self-dot-product
         * of the union feature vector
         */
        
        float total = 0;
        
        features_t::const_iterator it1 = c1.begin();
        features_t::const_iterator it2 = c2.begin();
        
        while(it1 != c1.end() && it2 != c2.end())
        {
            if(it1->first == it2->first)
            {
                float t = it1->second + it2->second;
                total += t * t;
                ++it1; ++it2;
            }
            else if(it1->first < it2->first)
            {
                total += it1->second * it1->second;
                ++it1;
            }
            else
            {
                total += it2->second * it2->second;
                ++it2;
            }
        }
        for(; it1 != c1.end(); ++it1)
            total += it1->second * it1->second;
        for(; it2 != c2.end(); ++it2)
            total += it2->second * it2->second;
        
        // Subtract self-similarities (eg, contributions where i == j)
        total -= c1_count + c2_count;
        // Normalize by cluster sizes
        float norm = (c1_count + c2_count) * (c1_count + c2_count - 1);
        return total / norm;
    }
    
    features_t merge_features(const features_t & c1, const features_t & c2)
    {
        // compute size of result vector
        unsigned feat_size = 0;
        
        features_t::const_iterator it1 = c1.begin();
        features_t::const_iterator it2 = c2.begin();
        
        while(it1 != c1.end() && it2 != c2.end())
        {
            if(it1->first == it2->first)
            { ++it1; ++it2; }
            else if(it1->first < it2->first)
                ++it1;
            else
                ++it2;
            
            feat_size += 1;
        }
        for(; it1 != c1.end(); ++it1, ++feat_size);
        for(; it2 != c2.end(); ++it2, ++feat_size);
        
        features_t fout; fout.reserve(feat_size);
        
        it1 = c1.begin(); it2 = c2.begin();
        
        while(it1 != c1.end() && it2 != c2.end())
        {
            if(it1->first == it2->first)
            {
                fout.push_back( features_t::value_type(
                    it1->first, it1->second + it2->second));
                ++it1; ++it2;
            }
            else if(it1->first < it2->first)
            {
                fout.push_back( features_t::value_type(
                    it1->first, it1->second));
                ++it1;
            }
            else
            {
                fout.push_back( features_t::value_type(
                    it2->first, it2->second));
                ++it2;
            }
        }
        for(; it1 != c1.end(); ++it1)
            fout.push_back( features_t::value_type(
                it1->first, it1->second));
        for(; it2 != c2.end(); ++it2)
            fout.push_back( features_t::value_type(
                it2->first, it2->second));
        
        return fout;
    }
    
    features_t extract_features(boost::python::object);
};

}; // end namespace clustering

#endif

