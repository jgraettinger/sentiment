
import getty

import vinz.estimation
import vinz.feature_transform
import vinz.features
import vinz.cluster
import vinz.normalization.basic_normalize
import vinz.normalization.newswire_normalize
import vinz.normalization.twitter_normalize
import vinz.featurization.basic_featurize

class Module(object):

    def configure(self, binder):

        binder.bind(vinz.featurization.InternTable,
            to = vinz.featurization.InternTable,
            scope = getty.Singleton)

        # twitter sample type
        binder.bind(vinz.normalization.Normalizer,
            to = vinz.normalization.twitter_normalize.TwitterNormalizer,
            with_annotation = 'twitter', scope = getty.Singleton)
        binder.bind(vinz.featurization.Featurizer,
            to = vinz.featurization.basic_featurize.TfFeaturizer,
            with_annotation = 'twitter', scope = getty.Singleton)

        # reuters sample type
        binder.bind(vinz.normalization.Normalizer,
            to = vinz.normalization.newswire_normalize.NewswireNormalizer,
            with_annotation = 'reuters', scope = getty.Singleton)
        binder.bind(vinz.featurization.Featurizer,
            to = vinz.featurization.basic_featurize.TfFeaturizer,
            with_annotation = 'reuters', scope = getty.Singleton)

        # flat, passed-through sample type
        binder.bind(vinz.normalization.Normalizer,
            to = vinz.normalization.basic_normalize.NoOpNormalizer,
            with_annotation = 'flat', scope = getty.Singleton)
        binder.bind(vinz.featurization.Featurizer,
            to = vinz.featurization.basic_featurize.PassthroughDenseFeaturizer,
            with_annotation = 'flat', scope = getty.Singleton)

        # clusterer bindings
        binder.bind(vinz.cluster.Clusterer,
            to = vinz.cluster.SparseGaussEmClusterer,
            with_annotation = 'SparseGaussEmClusterer')
        binder.bind(vinz.cluster.Clusterer,
            to = vinz.cluster.DenseGaussEmClusterer,
            with_annotation = 'DenseGaussEmClusterer')
        binder.bind(vinz.cluster.Clusterer,
            to = vinz.cluster.InnerProductClusterer,
            with_annotation = 'InnerProductClusterer')

        # feature-transform bindings
        binder.bind(vinz.feature_transform.FeatureTransform,
            to = vinz.feature_transform.PcaTransform,
            with_annotation = 'PcaTransform')
        binder.bind(vinz.feature_transform.FeatureTransform,
            to = vinz.feature_transform.IdfPcaTransform,
            with_annotation = 'IdfPcaTransform')
        binder.bind(vinz.feature_transform.FeatureTransform,
            to = vinz.feature_transform.IdfTransform,
            with_annotation = 'IdfTransform')
        binder.bind(vinz.feature_transform.FeatureTransform,
            to = vinz.feature_transform.RandomProjTransform,
            with_annotation = 'RandomProjTransform')

        # configuration parameters
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
            with_annotation = 'n_rand_output_features', to = 50)
        binder.bind_instance(getty.Config,
            with_annotation = 'n_pca_output_features', to = 25)
        binder.bind_instance(getty.Config,
            with_annotation = 'soft_prob_coeff_init', to = 0.10)
        binder.bind_instance(getty.Config,
            with_annotation = 'soft_prob_coeff_step', to = 0.10)
        binder.bind_instance(getty.Config,
            with_annotation = 'soft_prob_coeff_anneal', to = 0.50)

        binder.bind_instance(getty.Config,
            with_annotation = 'min_df_count', to = 5)
        binder.bind_instance(getty.Config,
            with_annotation = 'max_df_threshold', to = 0.90)

        return binder

