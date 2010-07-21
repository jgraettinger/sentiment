#ifndef CLUSTER_FEATURE_TRANSFORM_TRANSFORMS_HPP
#define CLUSTER_FEATURE_TRANSFORM_TRANSFORMS_HPP

#include "cluster/feature_transform/information_gain_statistic.hpp"
#include "cluster/feature_transform/statistic_cutoff_transform.hpp"
#include "cluster/feature_transform/random_projector_transform.hpp"
#include "cluster/feature_transform/chained_transform.hpp"

namespace cluster {
namespace feature_transform {

typedef statistic_cutoff_transform<
    information_gain_statistic
> igain_cutoff_transform;

typedef chained_transform<
    igain_cutoff_transform,
    random_projector_transform
> proj_igain_cutoff_transform;

};
};

#endif
