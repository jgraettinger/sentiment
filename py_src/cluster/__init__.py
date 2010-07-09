import common
from _cluster import *
del _cluster

import getty
import feature_selection

getty.Extension(NaiveBayesEmClusterer).requires(
    feature_selector = feature_selection.FeatureSelector)

# injection key
class Clusterer: pass

