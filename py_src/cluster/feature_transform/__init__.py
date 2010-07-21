from _feature_transform import *
del _feature_transform

import getty
import fwd

getty.Extension(InformationGainStatistic).requires(
    class_smoothing_factor = getty.Config('class_smoothing_factor'))

getty.Extension(IGainCutoffTransform).requires(
    min_features = getty.Config('min_features'),
    max_mass_ratio = getty.Config('max_mass_ratio'),
    max_features = getty.Config('max_features'),
    stat_provider = InformationGainStatistic)

getty.Extension(RandomProjTransform).requires(
    n_output_features = getty.Config('n_output_features'))

getty.Extension(ProjIGainCutoffTransform).requires(
    igain_cutoff_transform = IGainCutoffTransform,
    random_proj_transform = RandomProjTransform)

# injection key
class FeatureSelector: pass

