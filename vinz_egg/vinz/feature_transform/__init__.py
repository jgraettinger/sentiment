
import getty

import _feature_transform
from _feature_transform import *
#del _feature_transform

# injection key
class FeatureTransform: pass

getty.Extension(IdfTransform).requires(
    max_df_threshold = getty.Config('max_df_threshold'),
    min_df_count = getty.Config('min_df_count'))

getty.Extension(PcaTransform).requires(
    n_output_features = getty.Config('n_pca_output_features'))

getty.Extension(IdfPcaTransform).requires(
    idf_transform = IdfTransform,
    pca_transform = PcaTransform)

"""
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


getty.Extension(PCAProjIGainCutoffTransform).requires(
    proj_igain_cutoff_transform = ProjIGainCutoffTransform,
    pca_proj_transform = PCAProjTransform)

getty.Extension(PCACompactIGainCutoffTransform).requires(
    compact_igain_cutoff_transform = CompactIGainCutoffTransform,
    pca_proj_transform = PCAProjTransform)

getty.Extension(PCAIGainCutoffTransform).requires(
    igain_cutoff_transform = IGainCutoffTransform,
    pca_proj_transform = PCAProjTransform)


# 'is_static' is a meta-property which indicates whether the
#   transform is affected by cluster membership probabilities.
# Some transforms (such as PCA) use only the feature-space,
#   and therefore any re-training of the transform after the
#   probability space shifts is wasted effort.
# Other transforms (such as information gain) need to be
#   regularly retrained on updated membership probabilities.

IGainCutoffTransform.is_static = False
RandomProjTransform.is_static = True
ProjIGainCutoffTransform.is_static = False
CompactIGainCutoffTransform.is_static = False
PCAProjTransform.is_static = True
PCAProjIGainCutoffTransform.is_static = False
PCAIGainCutoffTransform.is_static = False
PCACompactIGainCutoffTransform.is_static = False
"""

IdfTransform.is_static = True
PcaTransform.is_static = True

# composite transforms
IdfPcaTransform.is_static = True

