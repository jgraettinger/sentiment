
from cluster import DenseGaussEmClusterer as Clusterer
from cluster.estimation import GaussianEstimator as Estimator
from cluster import DenseFeatures as Features

from matplotlib import *
from matplotlib.pyplot import *
from matplotlib.patches import Ellipse

import urllib
import random
import math
import sys
import re
import os

color = {
    'class_0': 'r',
    'class_1': 'g',
    'class_2': 'b',
    'class_3': 'cyan',
    'class_4': 'pink',
    'class_5': 'brown',
    'class_6': 'magenta',
}

#random.seed(36)

clus = Clusterer()

items = set()
estimators = {}
points = {}
n_classes = len(sys.argv) - 1

for c_ind in xrange(n_classes):
    uid = 'class_%d' % c_ind
    estimators[uid] = Estimator(2)
    clus.add_cluster('class_%d' % c_ind, estimators[uid])

for c_ind, dataset in enumerate(sys.argv[1:]):

    c_uid = 'class_%d' % c_ind

    cur_xs, cur_ys = points.setdefault(c_uid, ([], []))

    for l_ind, line in enumerate(open(dataset)):

        mem = {}
        for i in xrange(n_classes):
            mem['class_%d' % i] = [0, False]

        random.choice(mem.values())[0] = random.random()

        if l_ind < 2:
            mem[c_uid] = [1, True]

        d_uid = 'class_%d_%d' % (c_ind, l_ind)

        x, y = [float(i) for i in line.split()]

        clus.add_sample(
            d_uid,
            Features([x, y]),
            mem,
        )

        cur_xs.append(x)
        cur_ys.append(y)
        items.add(d_uid)


ion()
ax = gca()
xlim(-2, 30)
ylim(-2, 30)

for i in xrange(1500):
    print "entropy: %f" % clus.expect_and_maximize()

    ax.clear()

    for c_uid, (l_x, l_y) in points.items():
        plot(l_x, l_y, 'o', color = color[c_uid], alpha = 0.1)

    for c_uid, est in estimators.items():

        pos = est.get_mean()
        s0 = est.get_eigenvalue(0)
        s1 = est.get_eigenvalue(1)
        r0 = est.get_eigenvector(0)
        r1 = est.get_eigenvector(1)

        angle = math.atan2(r1[0], r0[0]) * 180.0 / math.pi
        e_plot = Ellipse(
            xy = pos,
            width  = 2.0 * math.sqrt(s0),
            height = 2.0 * math.sqrt(s1),
            angle  = angle,
            facecolor = color[c_uid],
            alpha = 0.5,
        )
        ax.add_patch(e_plot)

    draw()

"""
    groups = {}
    
    for s_uid in items:
        probs = clus.get_sample_probabilities(s_uid)
        p, c_uid = max([(j, i) for (i,j) in probs.items()])
        
        key = '%s:%s' % (s_uid[:7], c_uid)
        groups[key] = groups.get(key, 0) + 1
    
    print groups
"""

