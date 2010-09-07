#ifndef FEATURE_TRANSFORM_TRANSFORMS_HPP
#define FEATURE_TRANSFORM_TRANSFORMS_HPP

#include "feature_transform/information_gain_statistic.hpp"
#include "feature_transform/statistic_cutoff_transform.hpp"
#include "feature_transform/random_projector_transform.hpp"
#include "feature_transform/pca_projector_transform.hpp"
#include "feature_transform/compact_transform.hpp"
#include "feature_transform/chained_transform.hpp"

namespace feature_transform {

typedef statistic_cutoff_transform<
    information_gain_statistic
> igain_cutoff_transform;

typedef chained_transform<
    igain_cutoff_transform,
    random_projector_transform
> proj_igain_cutoff_transform;

typedef chained_transform<
    igain_cutoff_transform,
    compact_transform
> compact_igain_cutoff_transform;

typedef chained_transform<
    proj_igain_cutoff_transform,
    pca_projector_transform
> pca_proj_igain_cutoff_transform;

typedef chained_transform<
    compact_igain_cutoff_transform,
    pca_projector_transform
> pca_compact_igain_cutoff_transform;

typedef chained_transform<
    igain_cutoff_transform,
    pca_projector_transform
> pca_igain_cutoff_transform;

};

#endif
