
from cluster import DenseGaussEmClusterer as Clusterer
from cluster.estimation import GaussianEstimator as Estimator
from cluster import DenseFeatures as Features

from OpenGL.GLUT import * 
from OpenGL.GL import * 
from OpenGL.GLU import *

import urllib
import random
import math
import sys
import re
import os

clus = Clusterer()

fixed = set()

items = {} 
estimators = {}
n_classes = len(sys.argv) - 1

for c_ind in xrange(n_classes):
    uid = 'class_%d' % c_ind
    estimators[uid] = Estimator(2)
    clus.add_cluster('class_%d' % c_ind, estimators[uid])

for c_ind, dataset in enumerate(sys.argv[1:]):

    c_uid = 'class_%d' % c_ind

    for l_ind, line in enumerate(open(dataset)):

        mem = {}
        for i in xrange(n_classes):
            mem['class_%d' % i] = [0, False]

        #random.choice(mem.values())[0] = random.random()

        d_uid = 'class_%d_%d' % (c_ind, l_ind)

        if l_ind < 5:
            fixed.add(d_uid)
            mem[c_uid] = [1, True]
        
        x, y = [float(i) for i in line.split()]

        clus.add_sample(
            d_uid,
            Features([x, y]),
            mem,
        )

        items[d_uid] = c_uid

color = {
    'class_0': (1.0, 0.0, 0.0, 0.3),
    'class_1': (0.0, 1.0, 0.0, 0.3),
    'class_2': (0.0, 0.0, 1.0, 0.3),
    'class_3': (0.5, 0.5, 0.0, 0.3),
    'class_4': (0.0, 0.5, 0.5, 0.3),
    'class_5': (0.5, 0.0, 0.5, 0.3),
    'class_6': (0.3, 0.3, 0.3, 0.3),
}

def draw_clusters():
    
    for c_uid, est in estimators.items():

        glPushMatrix()

        pos = est.get_mean()
        s0 = est.get_eigenvalue(0)
        s1 = est.get_eigenvalue(1)

        r0 = est.get_eigenvector(0)
        r1 = est.get_eigenvector(1)

        angle = math.atan2(r1[0], r0[0]) * 180.0 / math.pi

        glTranslate(pos[0], pos[1], 0)
        glRotate(angle, 0, 0, 1)
        glScale( math.sqrt(s0), math.sqrt(s1), 0.1)

        glColor(*color[c_uid])

        glutSolidSphere(1, 20, 20)
        
        glPopMatrix()

def draw_samples():

    glBegin(GL_POINTS)

    for s_uid in items:

        pos = clus.get_estimator_features(s_uid).as_list()

        probs = clus.get_sample_probabilities(s_uid)

        p = clus.get_sample_likelihood(s_uid)
        cur_color = [0, 0, 0, math.exp(-100 * p)]
        for c_uid, (weight, hard) in probs.items():
            t = color[c_uid]
            cur_color[0] += t[0] * weight
            cur_color[1] += t[1] * weight
            cur_color[2] += t[2] * weight

        glColor(*cur_color)

        glVertex3f(pos[0], pos[1], 0)

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
        print "entropy: %f" % clus.expect_and_maximize()

    if args[0] == 'a':
        li, s_uid = min( (clus.get_sample_likelihood(i), i) for i in items if i not in fixed)
        print li, s_uid, len(fixed)

        fixed.add(s_uid)

        feat = clus.get_estimator_features(s_uid) 

        clus.drop_sample(s_uid)
        clus.add_sample(s_uid, feat, {items[s_uid]: (1, True)})

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

    print "entropy: %f" % clus.expect_and_maximize()
    glutDisplayFunc(draw)
    glutKeyboardFunc(keyboard)

    glMatrixMode(GL_PROJECTION)
    glOrtho(-1, 30, -1, 30, 1, -1)

    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    
    glutMainLoop()
    return

run('display')


