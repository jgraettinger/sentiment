#ifndef CLUSTER_ESTIMATION_GAUSSIAN_ESTIMATOR_HPP
#define CLUSTER_ESTIMATION_GAUSSIAN_ESTIMATOR_HPP

#include "cluster/dense_features.hpp"
#include <armadillo>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <cmath>

namespace cluster
{
namespace estimation
{

class gaussian_estimator
{
public:

    typedef boost::shared_ptr<gaussian_estimator> ptr_t;

    typedef dense_features features_t; 

    gaussian_estimator(unsigned n_features)
     : _n_features(n_features),
       _mean(n_features),
       _covar(n_features, n_features),
       _inv_covar(n_features, n_features)
    { reset(); }

    void reset()
    {
        _mean.zeros();
        _covar.zeros();

        _sample_mass = 0;
        _samples.clear();
    }

    void add_observation(
        const features_t::ptr_t & feat, double prob_class_sample)
    {
        if(feat->n_cols != _n_features)
            throw std::runtime_error("arity mismatch");

        if(prob_class_sample <= 0)
            return;

        _sample_mass += prob_class_sample;

        // add into cluster mean
        _mean += *feat * prob_class_sample;

        // hang on to the sample; we'll need it for co-variance
        _samples.push_back( std::make_pair(feat, prob_class_sample));
    }

    void prepare_estimator()
    {
        // normalize to obtain MLE mean
        _mean = _mean * (1.0 / _sample_mass);

//        _mean.print("mean: ");
//        std::cout << std::endl;

        // compute covariance matrix
        for(unsigned i = 0; i != _samples.size(); ++i)
        {
            const features_t & feat = *_samples[i].first;
            double  p_class_sample  = _samples[i].second; 

            // X - Xmean, scaled by class membership
            arma::rowvec vec = (feat - _mean) * p_class_sample;

            _covar += arma::trans(vec) * vec;
        }

        // normalize to obtain MLE covariance; to obtain an unbiased estimator,
        //  normalize by N - 1 (or, avg sample mass in this case)
        _covar *= 1.0 / _sample_mass;

        // Perform SVD over co-variance matrix
        arma::mat U, V;
        arma::colvec s;

        if( !arma::svd(U, s, V, _covar))
            throw std::runtime_error("SVD decomposition failed");

        // inverse(covar) = inverse(U x S x T*) = V x inv(S) x U*
        //_inv_covar = V * diagmat(1 / s) * arma::trans(U);
        _inv_covar = arma::inv(_covar);

        // determinant is product of eigenvalues
        double covar_det = arma::det(_covar);

        _gauss_norm  = std::pow(2 * M_PI, _n_features / 2.0);
        _gauss_norm *= std::sqrt(covar_det);
        _gauss_norm  = std::log(_gauss_norm);

/*
        _covar.print("covar: ");
        _inv_covar.print("inv_covar: ");
        arma::mat(_covar * _inv_covar).print("covar * inv_covar: ");
        std::cout << "covar determinant:\n  " << covar_det << std::endl;
        s.print("eigenvalues: ");
        U.print("U: ");
        V.print("V: ");

        std::cout << "log gauss norm:\n  " << _gauss_norm << std::endl;
*/
        return;
    }

    double estimate(const features_t::ptr_t & fptr)
    {
        if(fptr->n_cols != _n_features)
            throw std::runtime_error("arity mismatch");

        arma::rowvec t = *fptr - _mean;

        double mahalanobis_dist = arma::as_scalar(
            t * _inv_covar * arma::trans(t));

        return -0.5 * mahalanobis_dist - _gauss_norm;
    }

private:

    const unsigned _n_features;

    // MLE of distribution mean 
    arma::rowvec _mean;
   
    // MLE of covariance matrix, & it's inverse
    arma::mat    _covar;
    arma::mat    _inv_covar;

    // normalizing constant:
    // 1.0 / ( sqrt(det(covar)) * (2 * pi) ^ (D/2) )
    double _gauss_norm;

    double _sample_mass;
    std::vector<std::pair<features_t::ptr_t, double> > _samples;
};

};
};

#endif
