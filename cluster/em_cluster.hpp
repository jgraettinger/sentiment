
#ifndef EM_CLUSTER_HPP
#define EM_CLUSTER_HPP

#include "cluster/estimator.hpp"
#include "cluster/sample.hpp"
#include <boost/python.hpp>
#include <vector>
#include <map>

namespace cluster
{
namespace bpl = boost::python;

class em_cluster
{
public:

    em_cluster() {}

    void add_cluster(
        const std::string & cluster_uid,
        estimator::ptr_t estimator
    );

    void drop_cluster(
        const std::string & cluster_uid);

    void add_sample(
        sample::ptr_t sample,
        bpl::object soft_clusters,
        bpl::object hard_clusters
    );

    void drop_sample(const std::string & uid);

    bpl::object get_sample_probabilities(
        const std::string & sample_uid);

    float iterate();

private:

    struct sample_cluster_state_t {

        sample_features::ptr_t features;

        bool is_hard;

        float prob_sample_class;
        float prob_class_sample;
    };

    typedef std::vector<sample_cluster_state_t> sample_cluster_states_t;

    struct sample_state_t {

        sample::ptr_t sampl;

        sample_cluster_states_t cluster_states;
    };

    typedef std::map<std::string, sample_state_t> samples_t;

    void compute_pclass_norms(
        sample_cluster_states_t & states,
        float & hard_norm, float & soft_norm);

    std::vector<std::string> _clusters;
    std::vector<estimator::ptr_t> _cluster_estimators;

    samples_t _samples;
};

};

#endif

