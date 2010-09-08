
class ClusteringCoordinator(object):

    def __init__(self):

    def add_sample(
        self,
        sample_id,
        sample_type,
        sample_mass,
        cluster_state,
        **attributes):

    def drop_sample(self, sample_id):

    def add_cluster(self, cluster_id):

    def anneal_cluster(self, cluster_id):

    def drop_cluster(self, cluster_id):

    def expect_and_maximize(self):

    def sample_entropy(self, sample_id = None):

    def sample_state(self, sample_id = None):


