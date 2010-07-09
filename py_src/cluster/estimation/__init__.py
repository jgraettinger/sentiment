from _estimation import *
del _estimation

import getty
import fwd

getty.Extension(NaiveBayesEstimator).requires(
    alpha = getty.Config('alpha'))

# injection key
class Estimator: pass

