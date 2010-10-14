
#include "util.hpp"

// meta-classes for setting est_features iff
//  it's the same type as input_features
namespace {
    template<bool eq_feat_types>
    struct _est_features_init
    {
        template<typename Fi, typename Fe>
        void operator()(Fe & fe, const Fi & fi)
        { fe = fi; }
    };

    template<>
    struct _est_features_init<false>
    {
        template<typename Fi, typename Fe>
        void operator()(Fe & fe, const Fi & fi)
        { }
    };
};

template<typename InputFeatures, typename EstimatorFeatures>
cluster_sample<InputFeatures, EstimatorFeatures>::cluster_sample(
    const std::string & uid,
    const typename input_features_t::ptr_t & input_features,
    const cluster_set::ptr_t & cluster_set)
 :  uid(uid),
    prob_sample_cluster(cluster_set->size(), 0),
    prob_cluster_sample(cluster_set->size(), 0),
    is_hard(cluster_set->size(), false),
    weight(1.0),
    input_features(input_features),
    prob_sample(0),
    _cluster_set(cluster_set)
{
    // iff types are the same, set est_features to input_features
    _est_features_init< boost::is_same<
            input_features_t, estimator_features_t
        >::value>()(est_features, input_features);
}

template<typename InputFeatures, typename EstimatorFeatures>
void cluster_sample<InputFeatures, EstimatorFeatures
    >::notify_of_added_cluster(size_t index)
{
    prob_sample_cluster.insert(prob_sample_cluster.begin() + index, 0);
    prob_cluster_sample.insert(prob_cluster_sample.begin() + index, 0);
    is_hard.insert(is_hard.begin() + index, false);

    // no prob added, no need to re-normalize
    return;
}

template<typename InputFeatures, typename EstimatorFeatures>
void cluster_sample<InputFeatures, EstimatorFeatures
    >::notify_of_dropped_cluster(size_t index)
{
    prob_sample_cluster.erase(prob_sample_cluster.begin() + index);
    prob_cluster_sample.erase(prob_cluster_sample.begin() + index);
    is_hard.erase(is_hard.begin() + index);

    // renormalize P(cluster | sample) to 1
    norm_cluster_probs();
    return;
}

template<typename InputFeatures, typename EstimatorFeatures>
boost::python::object cluster_sample<InputFeatures, EstimatorFeatures
    >::get_cluster_probabilities()
{
    const std::vector<std::string> & cluster_uids(_cluster_set->get_uids());
    assert(cluster_uids.size() == prob_cluster_sample.size());

    boost::python::dict prob;
    for(size_t i = 0; i != cluster_uids.size(); ++i)
    {
        prob[cluster_uids[i]] = boost::python::make_tuple(
            prob_cluster_sample[i], (bool)is_hard[i]);
    }
    return prob;
}

template<typename InputFeatures, typename EstimatorFeatures>
void cluster_sample<InputFeatures, EstimatorFeatures
    >::set_cluster_probabilities(boost::python::object prob)
{
    std::vector<double> tmp_prob_cluster_sample(_cluster_set->size(), 0);
    std::vector<bool> tmp_is_hard(_cluster_set->size(), false);

    boost::python::object item;
    for(boost::python::object iter = get_iterator(prob); next(iter, item);)
    {
        // this will throw if the cluster-uid doesn't exist
        size_t c_ind = _cluster_set->get_cluster_index(
            boost::python::extract<std::string>(item[0]));

        // this will throw if item is of wrong type
        tmp_prob_cluster_sample[c_ind] = boost::python::extract<
            double>(item[1][0]);
        is_hard[c_ind] = boost::python::extract<
            bool>(item[1][1]);
    }
    // atomically update sample on success
    prob_cluster_sample.swap(tmp_prob_cluster_sample);
    is_hard.swap(tmp_is_hard);
    norm_cluster_probs();
    return;
}

template<typename InputFeatures, typename EstimatorFeatures>
void cluster_sample<InputFeatures, EstimatorFeatures
    >::norm_cluster_probs()
{
    // compute sum{ P(cluster | sample) } (for normalization)
    double soft_norm = 0;
    double hard_norm = 0;

    for(size_t i = 0; i != prob_cluster_sample.size(); ++i)
    {
        if(is_hard[i])
            hard_norm += prob_cluster_sample[i];
        else
            soft_norm += prob_cluster_sample[i];
    }

    if(soft_norm && hard_norm < 1)
    {
        soft_norm = (1 - hard_norm) / soft_norm;
        hard_norm = 1;
    }
    else if(hard_norm)
    {
        soft_norm = 0;
        hard_norm = 1 / hard_norm;
    }
    else
    {
        soft_norm = hard_norm = 1;
    }

    for(size_t i = 0; i != prob_cluster_sample.size(); ++i)
        prob_cluster_sample[i] *= is_hard[i] ? hard_norm : soft_norm;

    return;
}
