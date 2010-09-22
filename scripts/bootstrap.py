
import getty

import vinz
import vinz.featurization.basic_featurize
import vinz.normalization.basic_normalize
import vinz.normalization.twitter_normalize
import vinz.estimation
import vinz.feature_transform

def bootstrap():

    inj = getty.Injector()

    inj.bind(vinz.featurization.InternTable,
        to = vinz.featurization.InternTable,
        scope = getty.Singleton)

    inj.bind(vinz.featurization.Featurizer,
        to = vinz.featurization.basic_featurize.TfIdfFeaturizer)

    inj.bind(vinz.normalization.Normalizer,
        to = vinz.normalization.twitter_normalize.TwitterNormalizer)

    inj.bind(vinz.feature_transform.FeatureSelector,
        to = vinz.feature_transform.ProjIGainCutoffTransform)

    inj.bind_instance(getty.Config,
        with_annotation = 'alpha', to = 0.04)
    inj.bind_instance(getty.Config,
        with_annotation = 'min_features', to = 100)
    inj.bind_instance(getty.Config,
        with_annotation = 'max_mass_ratio', to = 0.75)
    inj.bind_instance(getty.Config,
        with_annotation = 'max_features', to = 100000)
    inj.bind_instance(getty.Config,
        with_annotation = 'class_smoothing_factor', to = 0.02)
    inj.bind_instance(getty.Config,
        with_annotation = 'n_rand_output_features', to = 300)
    inj.bind_instance(getty.Config,
        with_annotation = 'n_pca_output_features', to = 50)
    
    return inj

