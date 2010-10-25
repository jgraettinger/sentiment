#ifndef FEATURE_TRANSFORM_PCA_PROJECTOR_TRANSFORM_HPP
#define FEATURE_TRANSFORM_PCA_PROJECTOR_TRANSFORM_HPP

#include "features/dense_features.hpp"
#include "features/traits.hpp"
#include <boost/functional/hash.hpp>

#include <iostream>

namespace feature_transform {

class pca_projector_transform
{
public:

    typedef boost::shared_ptr<pca_projector_transform> ptr_t;

    pca_projector_transform(unsigned n_output_features)
     : _n_output_features(n_output_features) 
    { }

    template<typename InputFeatures>
    struct train_transform;

    template<typename InputFeatures>
    struct transform;

    double get_eigenvalue(unsigned i)
    {
        if(i < 0 || i >= _n_output_features)
            throw std::runtime_error("range error");

        return _svd_s[i];
    }

    std::vector<std::pair<unsigned, double> > get_eigenvector(unsigned i)
    {
        if(i < 0 || i >= _n_output_features)
            throw std::runtime_error("range error");

        typedef std::vector<std::pair<unsigned, double> > out_t;
        out_t out;

        if(_sparse_eigvecs.empty())
        {
            // learned a transform over dense inputs
            for(unsigned j = 0; j != _svd_U.n_cols; ++j)
                out[j] = std::make_pair(j, _svd_U(i, j));
        }
        else
        {
            out.insert(out.end(),
                _sparse_eigvecs[i].begin(), _sparse_eigvecs[i].end());
        }
        return out;
    }

private:

    arma::rowvec _mean;
    arma::mat    _covar;
    arma::mat    _svd_U, _svd_V;
    arma::colvec _svd_s;

    // for efficiency of computing dot-products in the
    //  sparse-input case, store eigenvectors as hash-maps
    typedef boost::unordered_map<unsigned, double> feature_hash_t;
    std::vector<feature_hash_t> _sparse_eigvecs;

    // projection of feature-space origin (after mean-shifting) onto eigenvecs
    std::vector<double> _sparse_mean_eigproj;

    const unsigned _n_output_features;
};

template<>
struct pca_projector_transform::train_transform<features::dense_features>
{
    ptr_t self;
    train_transform(const ptr_t & self) : self(self) {}

    typedef features::dense_features input_features_t;

    template<typename FeaturesMembershipsZipIterator>
    void operator()(
        const FeaturesMembershipsZipIterator & begin,
        const FeaturesMembershipsZipIterator & end)
    {
        input_features_t::ptr_t tmp = boost::get<0>(*begin);
        unsigned n_input_features = tmp->n_cols;
        assert(n_input_features >= self->_n_output_features);
 
        self->_mean = arma::zeros(1, n_input_features);
        self->_covar = arma::zeros(n_input_features, n_input_features);

        unsigned n_input_samples = 0;

        // compute mean of training samples
        for(FeaturesMembershipsZipIterator it = begin;
            it != end; ++it, ++n_input_samples)
        {
            self->_mean += *boost::get<0>(*it); 
        }

        self->_mean /= (double) n_input_samples;

        // compute covariance of training samples
        for(FeaturesMembershipsZipIterator it = begin; it != end; ++it)
        {
            arma::rowvec t = *boost::get<0>(*it) - self->_mean;
            self->_covar += arma::trans(t) * (t);
        }

        // we assume a normal distribution; otherwise we'd want to subtract one
        self->_covar /= (double) n_input_samples;

        //self->_covar.print(std::cout, "covar-matrix");

        // Perform SVD over co-variance matrix
        if( !arma::svd(self->_svd_U, self->_svd_s, self->_svd_V, self->_covar))
            throw std::runtime_error("SVD decomposition failed");

        //self->_svd_U.print(std::cout, "SVD-U");
        //self->_svd_s.print(std::cout, "SVD-s");
        //self->_svd_V.print(std::cout, "SVD-V");
/*
        // Debug
        unsigned t = 0;
        arma::mat temp = arma::zeros(n_input_samples, n_input_features);
        for(FeaturesMembershipsZipIterator it = begin;
            it != end; ++it, ++t)
        {
            temp.row(t) = *boost::get<0>(*it) - self->_mean;
        }

        arma::mat temp2 = temp * arma::trans(temp);
        temp2.print("T * T':");

        arma::mat Q, W;
        arma::colvec R;

        arma::svd(Q, R, W, temp2);

        Q.print("Q");
        R.print("R");
        W.print("W");
        
        arma::mat bar = arma::trans(temp) * Q;
        bar.print("T' * Q");
*/
        return;
    }
};

template<>
struct pca_projector_transform::transform<features::dense_features>
{
    ptr_t self;
    transform(const ptr_t & self) : self(self) {}

    typedef features::dense_features input_features_t;
    typedef features::dense_features output_features_t;
    typedef features::value<output_features_t>::type result_type;

    result_type operator()(
        const features::dense_features::ptr_t & ifeat) const
    {
        output_features_t::mutable_ptr_t of_ptr(
            new output_features_t(self->_n_output_features));
        output_features_t & ofeat(*of_ptr);

        for(unsigned i = 0; i != self->_n_output_features; ++i)
        {
            ofeat[i] = arma::as_scalar(*ifeat * self->_svd_U.col(i));
        }

        return of_ptr;
    }
};

template<>
struct pca_projector_transform::train_transform<features::sparse_features>
{
    ptr_t self;
    train_transform(const ptr_t & self) : self(self) {}

    typedef features::sparse_features input_features_t;

    template<typename FeaturesMembershipsZipIterator>
    void operator()(
        const FeaturesMembershipsZipIterator & begin,
        const FeaturesMembershipsZipIterator & end)
    {
        // mean of input feature vectors
        feature_hash_t mean;

        // flattened sequence of input features
        std::vector<input_features_t::ptr_t> inputs;

        for(FeaturesMembershipsZipIterator it = begin; it != end; ++it)
        {
            inputs.push_back( boost::get<0>(*it));

            // collect feature-id means
            features::iterator<input_features_t>::type f_it, f_end;
            f_it = inputs.back()->begin(); f_end = inputs.back()->end();

            for(; f_it != f_end; ++f_it)
                mean[f_it->first] += f_it->second;
        }

//        std::cout << inputs.size() << " input features" << std::endl;

        // inner product of mean vector against itself
        double dot_mean = 0;

        for(feature_hash_t::iterator it = mean.begin();
            it != mean.end(); ++it)
        {
            it->second /= inputs.size();
            dot_mean += it->second * it->second;
        }

        // debug
/*        std::cout << "mean: ";
        for(feature_hash_t::const_iterator it = mean.begin();
            it != mean.end(); ++it)
        {
            std::cout << it->first << ": " << it->second << ", ";
        }
        std::cout << std::endl;
        std::cout << "dot(mean, mean): " << dot_mean << std::endl;
*/

        // build covariance of samples (eg, scatter matrix)
        self->_covar = arma::eye(inputs.size(), inputs.size());
        for(unsigned i = 0; i != inputs.size(); ++i)
        {
            for(unsigned j = i; j != inputs.size(); ++j)
            {
                self->_covar(j, i) = self->_covar(i, j) = shifted_dot_product(
                    *inputs[i], *inputs[j], mean, dot_mean);
            }
        }

//        std::cout << "build covar, " << self->_covar.n_rows << ", ";
//        std::cout << self->_covar.n_cols << std::endl;
//        self->_covar.print(std::cout, "covar-matrix");

        // Perform SVD over co-variance matrix
        if( !arma::svd(self->_svd_U, self->_svd_s, self->_svd_V, self->_covar))
            throw std::runtime_error("SVD decomposition failed");

//        self->_svd_U.print(std::cout, "SVD-U");
//        self->_svd_s.print(std::cout, "SVD-s");
//        self->_svd_V.print(std::cout, "SVD-V");

        self->_sparse_eigvecs.resize(self->_n_output_features);
        for(unsigned i = 0; i != self->_n_output_features; ++i)
            self->_sparse_eigvecs[i].clear();

        // compute principal _n_output_features eigen-vectors
        for(unsigned input_ind = 0; input_ind != inputs.size(); ++input_ind)
        {
            features::iterator<input_features_t>::type f_it, f_end;
            f_it = inputs[input_ind]->begin(); f_end = inputs[input_ind]->end();

            for(; f_it != f_end; ++f_it)
            {
                unsigned f_id = f_it->first;
                double  f_val = f_it->second - mean[f_id];

                for(unsigned output_ind = 0;
                    output_ind != self->_n_output_features; ++output_ind)
                {
                    // update output eigenvector w/ this sample's
                    //  feature's weighted contribution to the vector
                    self->_sparse_eigvecs[output_ind][f_id] += \
                        f_val * self->_svd_U(input_ind, output_ind);
                }
            }
        }

        self->_sparse_mean_eigproj.resize(self->_n_output_features);

        // normalize eigen-vectors
        for(unsigned i = 0; i != self->_n_output_features; ++i)
        {
            feature_hash_t & hvec = self->_sparse_eigvecs[i];
            double norm = 1.0 / self->_svd_s[i];

            double mean_proj = 0;

            for(feature_hash_t::iterator it = hvec.begin(); it != hvec.end(); ++it)
            {
                it->second *= norm;

                mean_proj -= mean[it->first] * it->second;
            }
            self->_sparse_mean_eigproj[i] = mean_proj;
        }
/*
        for(unsigned i = 0; i != self->_n_output_features; ++i)
        {
            std::cout << "eigen " << i << std::endl;
        
            for(feature_hash_t::const_iterator it = self->_sparse_eigvecs[i].begin();
                it != self->_sparse_eigvecs[i].end(); ++it)
            {
                std::cout << it->first << ": " << it->second << ", ";
            }
            std::cout << std::endl;
        }
*/
        return;
    }

private:

    typedef std::vector< std::pair<unsigned, double> > vec_t;

    double shifted_dot_product(
        const vec_t & v1, const vec_t & v2,
        const feature_hash_t & mean, double dot_mean)
    {
        vec_t::const_iterator it1, it2;

        it1 = v1.begin();
        it2 = v2.begin();

        // start w/ the inner product of the mean vector
        //  against itself, and then update it for each
        //  feature captured by v1 and/or v2
        double prod = dot_mean;

        while(it1 != v1.end() && it2 != v2.end())
        {
            if(it1->first < it2->first)
            {
                const double & m = mean.find(it1->first)->second;

                prod += (it1->second - m) * m - m * m;
                ++it1;
            }
            else if(it1->first > it2->first)
            {
                const double & m = mean.find(it2->first)->second;

                prod += (it2->second - m) * m - m * m;
                ++it2;
            }
            else
            {
                const double & m = mean.find(it2->first)->second;

                prod += (it1->second - m) * (it2->second - m) - m * m;
                ++it1; ++it2;
            }
        }
        while(it1 != v1.end())
        {
            const double & m = mean.find(it1->first)->second;

            prod += (it1->second - m) * m - m * m;
            ++it1;
        }
        while(it2 != v2.end())
        {
            const double & m = mean.find(it2->first)->second;

            prod += (it2->second - m) * m - m * m;
            ++it2;
        }

        return prod;
    }
};

template<>
struct pca_projector_transform::transform<features::sparse_features>
{
    ptr_t self;
    transform(const ptr_t & self) : self(self) {}

    typedef features::sparse_features input_features_t;
    typedef features::dense_features output_features_t;
    typedef features::value<output_features_t>::type result_type;

    result_type operator()(
        const features::sparse_features::ptr_t & ifeat) const
    {
        output_features_t::mutable_ptr_t of_ptr(
            new output_features_t(self->_n_output_features));
        output_features_t & ofeat(*of_ptr);

        for(unsigned i = 0; i != self->_n_output_features; ++i)
        {
            const feature_hash_t & eigen_v = self->_sparse_eigvecs[i];
            double & proj_i = ofeat[i];

            // initialize w/ the projection onto the eigenvector
            //   of the origin shifted by the mean
            proj_i = self->_sparse_mean_eigproj[i];

            features::iterator<input_features_t>::type f_it, f_end;
            f_it = ifeat->begin(); f_end = ifeat->end();

            for(; f_it != f_end; ++f_it)
            {
                feature_hash_t::const_iterator e_it = eigen_v.find(f_it->first);
                if(e_it == eigen_v.end())
                    continue;

                // update w/ deviations from origin
                proj_i += e_it->second * f_it->second;
            }
        }

        return of_ptr;
    }
};

};

#endif
