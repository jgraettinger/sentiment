from _feature_transform import *
del _feature_transform

import getty

getty.Extension(InformationGainStatistic).requires(
    class_smoothing_factor = getty.Config('class_smoothing_factor'))

getty.Extension(IGainCutoffTransform).requires(
    min_features = getty.Config('min_features'),
    max_mass_ratio = getty.Config('max_mass_ratio'),
    max_features = getty.Config('max_features'),
    stat_provider = InformationGainStatistic)

getty.Extension(RandomProjTransform).requires(
    n_output_features = getty.Config('n_rand_output_features'))

getty.Extension(ProjIGainCutoffTransform).requires(
    igain_cutoff_transform = IGainCutoffTransform,
    random_proj_transform = RandomProjTransform)

getty.Extension(CompactIGainCutoffTransform).requires(
    igain_cutoff_transform = IGainCutoffTransform,
    compact_transform = CompactTransform)

getty.Extension(PCAProjTransform).requires(
    n_output_features = getty.Config('n_pca_output_features'))

getty.Extension(PCAProjIGainCutoffTransform).requires(
    proj_igain_cutoff_transform = ProjIGainCutoffTransform,
    pca_proj_transform = PCAProjTransform)

getty.Extension(PCACompactIGainCutoffTransform).requires(
    compact_igain_cutoff_transform = CompactIGainCutoffTransform,
    pca_proj_transform = PCAProjTransform)

getty.Extension(PCAIGainCutoffTransform).requires(
    igain_cutoff_transform = IGainCutoffTransform,
    pca_proj_transform = PCAProjTransform)

# injection key
class FeatureSelector: pass

