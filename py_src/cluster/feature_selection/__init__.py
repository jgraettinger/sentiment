from _feature_selection import *
del _feature_selection

import getty
import fwd

getty.Extension(InformationGainFilter).requires(
    min_features = getty.Config('min_features'),
    max_mass_ratio = getty.Config('max_mass_ratio'),
    max_features = getty.Config('max_features'),
    class_smoothing_factor = getty.Config('class_smoothing_factor'))

getty.Extension(InformationGainProjector).requires(
    min_features = getty.Config('min_features'),
    max_mass_ratio = getty.Config('max_mass_ratio'),
    max_features = getty.Config('max_features'),
    class_smoothing_factor = getty.Config('class_smoothing_factor'),
    n_output_features = getty.Config('n_output_features'))

# injection key
class FeatureSelector: pass

