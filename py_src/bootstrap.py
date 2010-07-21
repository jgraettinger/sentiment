
import cluster
import cluster.featurization.basic_featurize
import cluster.normalization.basic_normalize
import cluster.estimation
import cluster.feature_transform
import getty

def bootstrap():

    inj = getty.Injector()

    inj.bind(cluster.featurization.InternTable,
        to = cluster.featurization.InternTable,
        scope = getty.Singleton)

    inj.bind(cluster.featurization.Featurizer,
        to = cluster.featurization.basic_featurize.TfIdfFeaturizer)

    inj.bind(cluster.normalization.Normalizer,
        to = cluster.normalization.basic_normalize.BasicNormalizer)

    inj.bind(cluster.feature_transform.FeatureSelector,
        to = cluster.feature_transform.ProjIGainCutoffTransform)

#    inj.bind(cluster.Clusterer, to = cluster.NaiveBayesEmClusterer)

#    inj.bind(cluster.estimation.Estimator,
#        to = cluster.estimation.NaiveBayesEstimator)

    inj.bind_instance(getty.Config,
        with_annotation = 'alpha', to = 0.04)
    inj.bind_instance(getty.Config,
        with_annotation = 'min_features', to = 480)
    inj.bind_instance(getty.Config,
        with_annotation = 'max_mass_ratio', to = 0.70)
    inj.bind_instance(getty.Config,
        with_annotation = 'max_features', to = 100000)
    inj.bind_instance(getty.Config,
        with_annotation = 'class_smoothing_factor', to = 0.02)
    inj.bind_instance(getty.Config,
        with_annotation = 'n_output_features', to = 2)
    
    return inj

