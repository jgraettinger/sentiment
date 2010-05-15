#include "cluster/em_cluster.hpp"
#include "util.hpp"

#include <iostream>

namespace cluster
{
namespace bpl = boost::python;
using namespace std;

void em_cluster::add_cluster(
    const string & cluster_uid, estimator::ptr_t estimator)
{
    vector<string>::iterator it = lower_bound(
        _clusters.begin(), _clusters.end(), cluster_uid);

    if(it != _clusters.end() && *it != cluster_uid)
        throw runtime_error("duplicate cluster UID " + cluster_uid);

    unsigned ind = distance(_clusters.begin(), it);

    _clusters.insert(_clusters.begin() + ind, cluster_uid);
    _cluster_estimators.insert(_cluster_estimators.begin() + ind, estimator);

    for(samples_t::iterator it = _samples.begin(); it != _samples.end(); ++it)
    {
        sample_state_t & sample_state(it->second);

        sample_cluster_state_t ics;
        ics.prob_sample_class = 0;
        ics.prob_class_sample = 0;
        ics.features = estimator->add_sample(sample_state.sampl);

        sample_state.cluster_states.insert(
            sample_state.cluster_states.begin() + ind, ics);
    }
    return;
}

void em_cluster::drop_cluster(
    const string & cluster_uid)
{
    vector<string>::iterator it = lower_bound(
        _clusters.begin(), _clusters.end(), cluster_uid);

    if(it == _clusters.end() || *it != cluster_uid)
        throw runtime_error("no cluster with UID " + cluster_uid);

    unsigned ind = distance(_clusters.begin(), it);

    _clusters.erase(_clusters.begin() + ind);
    _cluster_estimators.erase(_cluster_estimators.begin() + ind);

    for(samples_t::iterator it = _samples.begin(); it != _samples.end(); ++it)
    {
        sample_cluster_states_t & states(it->second.cluster_states);
        states.erase(states.begin() + ind);
    }
    return;
}

void em_cluster::add_sample(
    sample::ptr_t sample,
    bpl::object soft_clusters,
    bpl::object hard_clusters
)
{
    if(_samples.find(sample->get_uid()) != _samples.end())
        throw runtime_error("duplicate sample UID " + sample->get_uid());

    sample_state_t & sample_state( _samples[sample->get_uid()]);
    sample_state.sampl = sample;
    sample_state.cluster_states.resize( _clusters.size());

    sample_cluster_states_t & states(sample_state.cluster_states);

    // extract sample features
    for(size_t i = 0; i != _clusters.size(); ++i)
    {
        states[i].is_hard = false;
        states[i].prob_sample_class = 0;
        states[i].prob_class_sample = 0;
        states[i].features = _cluster_estimators[i]->add_sample(sample);
    }

    // extract hard P(class | sample)
    for(bpl::object clus, iter = get_iterator(hard_clusters); next(iter, clus);)
    {
        string cluster_uid = bpl::extract<string>(clus[0])();
        double prob_class_sample = bpl::extract<double>(clus[1])();

        vector<string>::iterator it = lower_bound(
            _clusters.begin(), _clusters.end(), cluster_uid);

        if(it == _clusters.end() || *it != cluster_uid)
            throw runtime_error("no cluster with UID " + cluster_uid);

        sample_cluster_state_t & ics( states[
            distance(_clusters.begin(), it)]);

        ics.is_hard = true;
        ics.prob_sample_class = 0;
        ics.prob_class_sample = prob_class_sample;
    }

    // extract soft P(class | sample)
    for(bpl::object clus, iter = get_iterator(soft_clusters); next(iter, clus);)
    {
        string cluster_uid = bpl::extract<string>(clus[0])();
        double prob_class_sample = bpl::extract<double>(clus[1])();

        vector<string>::iterator it = lower_bound(
            _clusters.begin(), _clusters.end(), cluster_uid);

        if(it == _clusters.end() || *it != cluster_uid)
            throw runtime_error("no cluster with UID " + cluster_uid);

        sample_cluster_state_t & ics( states[
            distance(_clusters.begin(), it)]);

        // allow (but ignore) soft re-specification of hard clusters
        if(ics.is_hard)
            continue;

        ics.is_hard = false;
        ics.prob_sample_class = 0;
        ics.prob_class_sample = prob_class_sample;
    }
    return;
}

void em_cluster::drop_sample(const std::string & sample_uid)
{
    samples_t::iterator it = _samples.find(sample_uid);

    if(it == _samples.end())
        throw runtime_error("no sample with UID " + sample_uid);

    _samples.erase(it);
    return;
}

bpl::object em_cluster::get_sample_probabilities(const string & sample_uid)
{
    if(_samples.find(sample_uid) == _samples.end())
        throw runtime_error("no sample with UID " + sample_uid);

    sample_cluster_states_t & states( _samples[sample_uid].cluster_states);

    double hard_norm, soft_norm;
    compute_pclass_norms(states, hard_norm, soft_norm);

    bpl::dict probs;

    vector<string>::const_iterator it1 = _clusters.begin();
    sample_cluster_states_t::const_iterator it2 = states.begin();

    for(; it2 != states.end(); ++it1, ++it2)
    {
        probs[*it1] = it2->prob_class_sample;
    }
    return probs;
}

void em_cluster::compute_pclass_norms(
    em_cluster::sample_cluster_states_t & states,
    double & hard_norm, double & soft_norm)
{
    // compute sum{ P(class | sample) } (for normalization)
    soft_norm = 0; hard_norm = 0;
    for(size_t i = 0; i != states.size(); ++i)
    {
        if(states[i].is_hard)
            hard_norm += states[i].prob_class_sample;
        else
            soft_norm += states[i].prob_class_sample;
    }
    if(soft_norm && hard_norm < 1)
    {
        soft_norm = (1 - hard_norm) / soft_norm;
        hard_norm = 1;
    }
    else if(hard_norm)
    {
        soft_norm = 1;
        hard_norm = 1 / hard_norm;
    }
    else
    {
        soft_norm = hard_norm = 1;
    }
    return;
}

double em_cluster::iterate()
{
    // P(class)
    vector<double> cluster_probs(_clusters.size(), 0);

    for(size_t i = 0; i != _clusters.size(); ++i)
        _cluster_estimators[i]->reset_estimator();

    for(samples_t::iterator it = _samples.begin(); it != _samples.end(); ++it)
    {
        sample_cluster_states_t & states( it->second.cluster_states);
        
        double hard_norm, soft_norm;
        compute_pclass_norms(states, hard_norm, soft_norm);

        // feed sample & P(class | sample) to estimators
        for(size_t i = 0; i != states.size(); ++i)
        {
            double prob_class_sample = states[i].prob_class_sample * (
                states[i].is_hard ? hard_norm : soft_norm);

            _cluster_estimators[i]->add_sample_probability(
                states[i].features, prob_class_sample);

            // add P(class | sample) to P(class)
            cluster_probs[i] += prob_class_sample;
        }
    }

    for(size_t i = 0; i != _clusters.size(); ++i)
    {
        _cluster_estimators[i]->prepare_estimator();
        cluster_probs[i] /= _samples.size();
    }

    for(samples_t::iterator it = _samples.begin(); it != _samples.end(); ++it)
    {
        sample_cluster_states_t & states( it->second.cluster_states);

        // P(sample) = sum{ P(sample|class)}
        double prob_sample = 0;

        for(size_t i = 0; i != states.size(); ++i)
        {
            // extract P(sample|class) from estimators
            states[i].prob_sample_class = \
                _cluster_estimators[i]->estimate_sample(states[i].features);

            prob_sample += states[i].prob_sample_class;
        }

//        std::cout << "P(sample) " << prob_sample << std::endl;

        if(!prob_sample)
            prob_sample = 1;

        for(size_t i = 0; i != states.size(); ++i)
        {
//            std::cout << states[i].prob_class_sample << " => ";

            if(states[i].is_hard)
                continue;

            // P(class|sample) = P(class) * P(sample|class) / P(sample)
            states[i].prob_class_sample = \
                cluster_probs[i] * states[i].prob_sample_class / prob_sample;

//            std::cout << cluster_probs[i] << " * " << states[i].prob_sample_class;
//            std::cout << " / " << prob_sample << std::endl;


//            std::cout << states[i].prob_class_sample << ", ";
        }
//        std::cout << std::endl;
    }

    for(size_t i = 0; i != _clusters.size(); ++i)
        std::cout << cluster_probs[i] << " ";
    std::cout << std::endl;

    return 0;
}

void make_em_cluster_bindings()
{
    bpl::class_<em_cluster>("em_cluster", bpl::init<>())
    .def("add_cluster", &em_cluster::add_cluster)
    .def("drop_cluster", &em_cluster::drop_cluster)
    .def("add_sample", &em_cluster::add_sample)
    .def("drop_sample", &em_cluster::drop_sample)
    .def("get_sample_probabilities", &em_cluster::get_sample_probabilities)
    .def("iterate", &em_cluster::iterate);
}

};

