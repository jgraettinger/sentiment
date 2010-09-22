
import getty

from vinz.cluster import Clusterer
from vinz.normalization import Normalizer
from vinz.featurization import Featurizer

class ClusteringCoordinator(object):

    def __init__(self, clusterer, feature_transform, injector):

        self._clusterer = clusterer
        self._feature_transform = feature_transform
        self._injector = injector

        # parameters for controlling how strongly 'soft probability' should
        #  be factored into the clustering model during iteration
        self._soft_prob_coeff_init = injector.get_instance(
            getty.Config, annotation = 'soft_prob_coeff_init')
        self._soft_prob_coeff_step = injector.get_instance(
            getty.Config, annotation = 'soft_prob_coeff_step')
        self._soft_prob_coeff_anneal = injector.get_instance(
            getty.Config, annotation = 'soft_prob_coeff_anneal')

        # indicates whether samples have been added/removed
        #   since the last call to expect_and_maximize()
        self._samples_changed = False

        # {cluster-id: cluster-estimator}
        self._estimators = {}

        # set([sample-id])
        self._samples = set()
        return

    def add_sample(
        self,
        sample_id,
        sample_type,
        sample_mass,
        cluster_state,
        **sample):

        # use getty to identify the injected normalizer &
        #  featurizer for this type of sample
        normalizer = self._injector.get_instance(
            Normalizer, annotation = sample_type)
        featurizer = self._injector.get_instance(
            Featurizer, annotation = sample_type)

        # normalize & featurize the sample
        normalized_sample = normalizer.normalize(sample)
        sample_features = featurizer.featurize(normalized_sample)

        # hand-off to clusterer
        self._clusterer.add_sample(
            sample_id, sample_mass, sample_features, cluster_state)
        self._samples.add(sample_id)

        self._samples_changed = True
        return

    def drop_sample(self, sample_id):
        self._clusterer.drop_sample(sample_id)
        self._samples.discard(sample_id)
        
        self._samples_changed = True
        return

    def add_cluster(self, cluster_id):
        estimator = self._injector.get_instance(
            self._clusterer.estimator_type)
        self._clusterer.add_cluster(cluster_id, estimator)

        self._estimators[cluster_id] = estimator
        estimator.soft_prob_coeff = self._soft_prob_coeff_init
        return

    def drop_cluster(self, cluster_id):
        self._clusterer.drop_cluster(cluster_id)
        del self._estimators[cluster_id]
        return

    def anneal_cluster(self, cluster_id):
        self._estimators[cluster_id].soft_prob_coeff *= \
            self._soft_prob_coeff_anneal
        return

    def expect_and_maximize(self):

        if self._feature_transform:
            if not self._feature_transform.is_static or self._samples_changed:
                self._clusterer.transform_features(self._feature_transform)

        entropy = self._clusterer.expect_and_maximize()
        for estimator in self._estimators.values():
            estimator.soft_prob_coeff = min(1.0,
                estimator.soft_prob_coeff + self._soft_prob_coeff_step)
        return entropy

    def sample_state(self):

        for sample_id in self._samples:
            yield (
                sample_id,
                self._clusterer.get_sample_likelihood(sample_id),
                self._clusterer.get_sample_probabilities(sample_id))
        return

