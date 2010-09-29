
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
    vinz.cluster.Clusterer, annotation = 'dense_passthrough')
feature_transform = inj.get_instance(
    vinz.feature_transform.FeatureTransform, annotation = 'dense_passthrough')
coord = ClusteringCoordinator(clusterer, feature_transform, inj)


for c_ind, dataset in enumerate(sys.argv[1:]):

    class_id = 'class_%d' % c_ind
    coord.add_cluster(class_id)

    for l_ind, line in enumerate(open(dataset)):

        if l_ind < 2:
            mem = {class_id: (1, True)}
        else:
            mem = {}

        d_uid = 'class_%d_%d' % (c_ind, l_ind)

        x, y = [float(i) for i in line.split()]

        coord.add_sample(d_uid, 'dense_passthrough', 1, mem, features = [x, y])

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

    for s_uid, p, probs in coord.sample_state():

        is_fixed = any(i[1] for i in probs.values())

        if is_fixed:
            cur_color = [0, 0, 0, 1]
        else:
            # color-strength inverse to likelihood
            cur_color = [0, 0, 0, 0.5 * math.exp(-100 * p)]

        for c_uid, (weight, hard) in probs.items():
            t = color[c_uid]
            cur_color[0] += t[0] * weight
            cur_color[1] += t[1] * weight
            cur_color[2] += t[2] * weight

        glColor(*cur_color)

        x, y = coord.clusterer.get_estimator_features(s_uid).as_list()
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

    if args[0] == 'a':

        min_li, min_uid, min_cur_clus, min_act_clus = \
            None, None, None, None

        for cur_uid, cur_li, probs in coord.sample_state():

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

        coord.clusterer.set_sample_probabilities(
            min_uid, {min_act_clus: (1, True)})

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


