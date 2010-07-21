#ifndef CLUSTER_ESTIMATION_GAUSSIAN_ESTIMATOR_HPP
#define CLUSTER_ESTIMATION_GAUSSIAN_ESTIMATOR_HPP

#include "cluster/features/dense_features.hpp"
#include <armadillo>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <cmath>
#include <limits>

namespace cluster
{
namespace estimation
{

class gaussian_estimator
{
public:

    typedef boost::shared_ptr<gaussian_estimator> ptr_t;

    typedef features::dense_features features_t; 

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

    double prepare_estimator()
    {
        // normalize to obtain MLE mean
        _mean = _mean * (1.0 / _sample_mass);

//        _mean.print("mean: ");
//        std::cout << std::endl;

        double sq_std_dev = 0;

        // compute covariance matrix
        for(unsigned i = 0; i != _samples.size(); ++i)
        {
            const features_t & feat = *_samples[i].first;
            double  p_class_sample  = _samples[i].second; 

            // X - Xmean, scaled by class membership
            arma::rowvec vec = (feat - _mean) * p_class_sample;

            sq_std_dev += arma::as_scalar(vec * arma::trans(vec));
            _covar += arma::trans(vec) * vec;
        }

        // normalize to obtain MLE covariance & std-deviation
        sq_std_dev *= 1.0 / _sample_mass;
        _covar *= 1.0 / _sample_mass;

        double coeff = std::exp(-1.0 * _sample_mass / (_n_features + 1.0)); 

        // interpolate covariance matrix & a spherical co-variance,
        //   weighted by the quantity of sample-mass relative to the
        //   dimensionality of the distribution
        _covar = (1.0 - coeff) * _covar + coeff * sq_std_dev * \
            arma::eye(_covar.n_rows, _covar.n_cols);;

        // Perform SVD over co-variance matrix
        if( !arma::svd(_svd_U, _svd_s, _svd_V, _covar))
            throw std::runtime_error("SVD decomposition failed");

        // inverse(covar) = inverse(U x S x T*) = V x inv(S) x U*
        //_inv_covar = _svd_V * arma::diagmat(1.0 / _svd_s) * arma::trans(_svd_U);
        _inv_covar = arma::inv(_covar);

        // determinant is product of eigenvalues; scale by 2 * PI
        double scaled_det = 0;
        for(arma::colvec::const_iterator it = _svd_s.begin(); it != _svd_s.end(); ++it)
            scaled_det += 2.0 * M_PI * *it;

        _gauss_norm = std::sqrt(scaled_det);

        // return entropy of the distribution
        return std::log( std::sqrt(scaled_det * std::pow(M_E, _n_features)));

/*
        /// debug
        std::cout << "scaled_det:\n  " << scaled_det << std::endl;

        _covar.print("covar: ");
        _inv_covar.print("inv_covar: ");
        arma::mat(_covar * _inv_covar).print("covar * inv_covar: ");
        _svd_s.print("eigenvalues: ");
        _svd_U.print("U: ");
        _svd_V.print("V: ");

        std::cout << "gauss norm:\n  " << _gauss_norm << std::endl;
*/
    }

    double estimate(const features_t::ptr_t & fptr)
    {
        if(fptr->n_cols != _n_features)
            throw std::runtime_error("arity mismatch");

        if(_gauss_norm == 0)
            return -std::numeric_limits<double>::max();

        arma::rowvec t = *fptr - _mean;

        double mahalanobis_dist = arma::as_scalar(
            t * _inv_covar * arma::trans(t));

        double result = std::exp(-0.5 * mahalanobis_dist) / _gauss_norm;

        return result;
    }

    std::vector<double> get_mean()
    {
        std::vector<double> v(_mean.n_cols);
        std::copy(_mean.begin(), _mean.end(), v.begin());
        return v;
    }

    std::vector<double> get_eigenvector(unsigned i)
    {
        std::vector<double> v(_svd_U.n_cols); unsigned j = 0;
        for(arma::mat::row_iterator it = _svd_U.begin_row(i);
            it != _svd_U.end_row(i); ++it)
        {
            v[j++] = *it;
        }
        return v;
    }

    const double get_eigenvalue(unsigned i)
    { return _svd_s(i); }

private:

    const unsigned _n_features;

    // MLE of distribution mean 
    arma::rowvec _mean;
   
    // MLE of covariance matrix, & it's inverse
    arma::mat    _covar;
    arma::mat    _inv_covar;

    arma::mat    _svd_U, _svd_V;
    arma::colvec _svd_s;

    // normalizing constant:
    // 1.0 / ( sqrt(det(covar)) * (2 * pi) ^ (D/2) )
    double _gauss_norm;

    double _sample_mass;
    std::vector<std::pair<features_t::ptr_t, double> > _samples;
};

};
};

#endif
