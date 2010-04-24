
#ifndef CLUSTER_GROUP_AVG_SIMILARITY_HPP
#define CLUSTER_GROUP_AVG_SIMILARITY_HPP

#include <boost/python.hpp>
#include <vector>

namespace cluster {

struct group_average_similarity {
    
    // http://nlp.stanford.edu/IR-book/html/htmledition/
    //      group-average-agglomerative-clustering-1.html
    
    typedef std::vector< std::pair<unsigned, float> > features_t;
    
    inline float similarity(
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

        {
            float result = total / norm;
            if(result < 0 || result > 1.0)
                throw std::runtime_error("similarity range error"); 
        }
        return total / norm;
    }

    inline float distance(
        const features_t & c1, const features_t & c2,
        const float c1_count, const float c2_count
    ) const
    { return 1.0 - similarity(c1, c2, c1_count, c2_count); }

    inline void merge_features_into(
        const features_t & c1, float c1_mass,
        const features_t & c2, float c2_mass,
        features_t & fout)
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
        
        fout.clear(); fout.reserve(feat_size);
        
        it1 = c1.begin(); it2 = c2.begin();
        
        while(it1 != c1.end() && it2 != c2.end())
        {
            if(it1->first == it2->first)
            {
                fout.push_back( features_t::value_type(
                    it1->first, c1_mass * it1->second + c2_mass * it2->second));
                ++it1; ++it2;
            }
            else if(it1->first < it2->first)
            {
                fout.push_back( features_t::value_type(
                    it1->first, c1_mass * it1->second));
                ++it1;
            }
            else
            {
                fout.push_back( features_t::value_type(
                    it2->first, c2_mass * it2->second));
                ++it2;
            }
        }
        for(; it1 != c1.end(); ++it1)
            fout.push_back( features_t::value_type(
                it1->first, c1_mass * it1->second));
        for(; it2 != c2.end(); ++it2)
            fout.push_back( features_t::value_type(
                it2->first, c2_mass * it2->second));
        
        return;
    }
    
    inline features_t merge_features(
        const features_t & c1, float c1_mass,
        const features_t & c2, float c2_mass)
    {
        features_t temp;
        merge_features_into(
            c1, c1_mass, c2, c2_mass, temp);
        return temp;
    }



    void extract_features(boost::python::object, features_t & into);
};

}; // end namespace clustering

#endif

