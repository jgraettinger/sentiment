
import getty

import vinz.estimation
import vinz.feature_transform
import vinz.features
import vinz.cluster
import vinz.featurization.basic_featurize
import vinz.normalization.basic_normalize
import vinz.normalization.twitter_normalize

class Module(object):

    def configure(self, binder):

        binder.bind(vinz.featurization.InternTable,
            to = vinz.featurization.InternTable,
            scope = getty.Singleton)

        # twitter sample type
        binder.bind(vinz.featurization.Featurizer,
            to = vinz.featurization.basic_featurize.TfFeaturizer,
            with_annotation = 'twitter', scope = getty.Singleton)
        binder.bind(vinz.normalization.Normalizer,
            to = vinz.normalization.twitter_normalize.TwitterNormalizer,
            with_annotation = 'twitter', scope = getty.Singleton)

        # dense_passthrough sample type
        binder.bind(vinz.featurization.Featurizer,
            to = vinz.featurization.basic_featurize.PassthroughDenseFeaturizer,
            with_annotation = 'dense_passthrough', scope = getty.Singleton)
        binder.bind(vinz.normalization.Normalizer,
            to = vinz.normalization.basic_normalize.NoOpNormalizer,
            with_annotation = 'dense_passthrough', scope = getty.Singleton)

        # web_document clustering configuration
        binder.bind(vinz.cluster.Clusterer,
            to = vinz.cluster.SparseGaussEmClusterer,
            with_annotation = 'web_document')
        binder.bind(vinz.feature_transform.FeatureTransform,
            to = vinz.feature_transform.PCAProjTransform,
            with_annotation = 'web_document') 

        # dense_passthrough clustering configuration
        binder.bind(vinz.cluster.Clusterer,
            to = vinz.cluster.DenseGaussEmClusterer,
            with_annotation = 'dense_passthrough')
        binder.bind_instance(vinz.feature_transform.FeatureTransform,
            to = None, with_annotation = 'dense_passthrough')

        # system parameters
        binder.bind_instance(getty.Config,
            with_annotation = 'alpha', to = 0.04)
        binder.bind_instance(getty.Config,
            with_annotation = 'min_features', to = 100)
        binder.bind_instance(getty.Config,
            with_annotation = 'max_mass_ratio', to = 0.75)
        binder.bind_instance(getty.Config,
            with_annotation = 'max_features', to = 100000)
        binder.bind_instance(getty.Config,
            with_annotation = 'class_smoothing_factor', to = 0.02)
        binder.bind_instance(getty.Config,
            with_annotation = 'n_rand_output_features', to = 300)
        binder.bind_instance(getty.Config,
            with_annotation = 'n_pca_output_features', to = 50)
        binder.bind_instance(getty.Config,
            with_annotation = 'soft_prob_coeff_init', to = 0.05)
        binder.bind_instance(getty.Config,
            with_annotation = 'soft_prob_coeff_step', to = 0.03)
        binder.bind_instance(getty.Config,
            with_annotation = 'soft_prob_coeff_anneal', to = 0.5)

        return binder

