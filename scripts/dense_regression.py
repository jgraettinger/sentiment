
import math
import sys
import getty

from OpenGL.GLUT import * 
from OpenGL.GL import * 
from OpenGL.GLU import *

import vinz.module
from vinz.cluster import Clusterer
from vinz.feature_transform import FeatureTransform
from vinz.clustering_coordinator import ClusteringCoordinator


inj = vinz.module.Module().configure(getty.Injector())

clusterer = inj.get_instance(
    vinz.cluster.Clusterer, annotation = 'DenseGaussEmClusterer')
feature_transform = None
coord = ClusteringCoordinator(clusterer, feature_transform, inj)


for c_ind, dataset in enumerate(sys.argv[1:]):

    class_id = 'class_%d' % c_ind
    coord.add_cluster(class_id)

    for l_ind, line in enumerate(open(dataset)):

        d_uid = 'class_%d_%d' % (c_ind, l_ind)
        x, y = [float(i) for i in line.split()]

        sample = coord.add_sample(d_uid,
            'flat', features = [x, y])

        if l_ind < 2:
            sample.cluster_probabilities = {class_id: (1, True)}

color = {
    'class_0': (1.0, 0.0, 0.0, 0.3),
    'class_1': (0.0, 1.0, 0.0, 0.3),
    'class_2': (0.0, 0.0, 1.0, 0.3),
    'class_3': (0.5, 0.5, 0.0, 0.3),
    'class_4': (0.0, 0.5, 0.5, 0.3),
    'class_5': (0.5, 0.0, 0.5, 0.3),
    'class_6': (0.3, 0.3, 0.3, 0.3),
    'class_7': (0.2, 0.3, 0.4, 0.3),
}

def draw_clusters():
    
    for c_uid, est in coord.estimators.items():

        glPushMatrix()

        pos = est.get_mean()
        s0 = est.get_eigenvalue(0)
        s1 = est.get_eigenvalue(1)

        r0 = est.get_eigenvector(0)
        r1 = est.get_eigenvector(1)

        angle = math.atan2(r1[0], r0[0]) * 180.0 / math.pi

        glTranslate(pos[0], pos[1], 0)
        glRotate(angle, 0, 0, 1)
        glScale(math.sqrt(s0), math.sqrt(s1), 0.1)

        glColor(*color[c_uid])

        glutSolidSphere(1, 20, 20)
        
        glPopMatrix()

def draw_samples():

    glBegin(GL_POINTS)

    for sample in coord.samples:

        is_fixed = any(i[1] for i in sample.cluster_probabilities.values())

        if is_fixed:
            cur_color = [0, 0, 0, 1]
        else:
            # color-strength inverse to likelihood
            cur_color = [0, 0, 0, 0.5 * math.exp(-100 * sample.probability)]

        for c_uid, (weight, hard) in sample.cluster_probabilities.items():
            t = color[c_uid]
            cur_color[0] += t[0] * weight
            cur_color[1] += t[1] * weight
            cur_color[2] += t[2] * weight

        glColor(*cur_color)

        x, y = sample.estimator_features.as_list()
        glVertex3f(x, y, 0)

    glEnd()

def draw():

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glPushMatrix()
    
    draw_clusters()
    draw_samples()

    glPopMatrix()
    glutSwapBuffers()
    return

def keyboard(*args):

    if args[0] == ' ':
        print "entropy: %f" % coord.expect_and_maximize()
        tot_prob = sum(math.log(s.probability) for s in coord.clusterer.samples)
        print "total prob: %f" % tot_prob

    if args[0] == 'a':

        min_li, min_uid, min_cur_clus, min_act_clus = \
            None, None, None, None

        for sample in coord.samples:

            cur_uid = sample.uid
            cur_li = sample.probability
            probs = sample.cluster_probabilities
                
            # it's already fixed
            if any(i[1] for i in probs.values()):
                continue

            cur_clus = max((j, i) for i, j in probs.items())[1]
            act_clus = 'class_%s' % cur_uid.split('_')[1]

            # not misclassified
            if cur_clus == act_clus:
                continue

            if min_li is None or cur_li < min_li:
                min_uid = cur_uid
                min_li = cur_li
                min_cur_clus = cur_clus
                min_act_clus = act_clus

        print min_uid, min_li

        coord.clusterer.get_sample(min_uid
            ).cluster_probabilities = {min_act_clus: (1, True)}

        if min_act_clus != min_cur_clus:
            coord.anneal_cluster(min_cur_clus)
            coord.anneal_cluster(min_act_clus)

    draw()

def run(name):

    glutInit([])
    glutCreateWindow('foobar')
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH)

    glPointSize(3)
    glEnable(GL_POINT_SMOOTH)

    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    glClearColor(0, 0, 0, 1)
    glShadeModel(GL_SMOOTH)
    glEnable(GL_CULL_FACE)
    glEnable(GL_DEPTH_TEST)

    print "entropy: %f" % coord.expect_and_maximize()
    glutDisplayFunc(draw)
    glutKeyboardFunc(keyboard)

    glMatrixMode(GL_PROJECTION)
    glOrtho(-1, 30, -1, 30, 1, -1)

    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    
    glutMainLoop()
    return

run('display')

"""
import numpy, math
from numpy import linalg as la

class Foo(object):
    def __init__(self, mean, covar):
        self.mean = mean
        self.covar = covar
        self.p = 1.0 * len(mean)

    def pdf(self, feat):
        
        feat = [numpy.array(feat)]
        # initial part of the formula 
        # this code depends only on the model parameters ... optmize? 
        dd = la.det(self.covar)
        inverse = la.inv(self.covar); 
        ff = math.pow(2*math.pi,-self.p/2.0)*math.pow(dd,-0.5); 

        # centered input values 
        centered = numpy.subtract(
            feat,numpy.repeat([self.mean], len(feat), axis=0)) 

        res = ff * numpy.exp(-0.5*numpy.sum(
            numpy.multiply(centered,numpy.dot(centered,inverse)),1)) 

        return res[0]

while True:
    print "entropy: %f" % coord.expect_and_maximize()
    tot_prob = sum(math.log(s.probability) for s in coord.clusterer.samples)
    print "total prob: %f" % tot_prob

    estimators = coord.estimators.values()

    oest = [Foo(e.get_mean(), e.get_covar()) for e in estimators]

    for sample in coord.clusterer.samples:
        for e, oe in zip(estimators, oest):
            r_e = e.estimate(sample.estimator_features)
            r_oe = oe.pdf(sample.estimator_features.as_list())
            assert round(r_e,4) == round(r_oe,4), '%r == %r' % (r_e, r_oe)
"""

