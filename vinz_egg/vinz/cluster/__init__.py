
import vinz.estimation

from _cluster import *
del _cluster

# injection key
class Clusterer: pass

DenseGaussEmClusterer.estimator_type = vinz.estimation.GaussianEstimator
SparseGaussEmClusterer.estimator_type = vinz.estimation.GaussianEstimator

