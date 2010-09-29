
import sys
import math
import simplejson
import sqlite3
import getty
import random

from OpenGL.GLUT import * 
from OpenGL.GL import * 
from OpenGL.GLU import *

import vinz.module
from vinz.cluster import Clusterer
from vinz.feature_transform import FeatureTransform
from vinz.clustering_coordinator import ClusteringCoordinator

random.seed(32)

if len(sys.argv) < 6:
    print "Usage: %s <regression-db> <clusterer-type> <feature-transform-type>"\
        " <class-id> <class-id> ..." % sys.argv[0]
    sys.exit(-1)

db = sqlite3.connect(sys.argv[1], isolation_level = None)

inj = vinz.module.Module().configure(getty.Injector())
clusterer = inj.get_instance(
    vinz.cluster.Clusterer, annotation = sys.argv[2])
feature_transform = inj.get_instance(
    vinz.feature_transform.FeatureTransform, annotation = sys.argv[3])
coord = ClusteringCoordinator(clusterer, feature_transform, inj)

# {sample-uid: [class-id]}
sample_classes = {}
# {class-id: # of samples}
class_sample_size = {}
# {class-id: [r, g, b, a]}
class_color = {}

for class_id in sys.argv[4:]:

    print "Loading samples for class %s" % class_id
    class_sample_size[class_id] = 0

    coord.add_cluster(class_id)

    for uid, type, weight, attributes in db.execute("""
        select uid, type, weight, attributes from sample join sample_class on
            uid = sample_uid and class_name = ?""", (class_id,)):

        uid = str(uid)

        if uid not in sample_classes:
            sample_classes[uid] = []
            attributes = simplejson.loads(attributes)

            if class_sample_size[class_id] < 200:
                s_prob = {class_id: (1.0, True)}
            else:
                s_prob = {}

            coord.add_sample(uid, type, weight, s_prob, **attributes)

        sample_classes[uid].append(class_id)
        class_sample_size[class_id] += 1.0

class_color = dict((class_id, [
    random.random(), random.random(), random.random(), 0.1]) \
    for class_id in class_sample_size)

coord.transform_features()


def draw():

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    glBegin(GL_POINTS)

    for uid in coord.samples:

        glColor(*class_color[sample_classes[uid][0]])

        feats = coord.clusterer.get_estimator_features(uid).as_list()
        for y, x in enumerate(feats):
            glVertex3f(x, y, 0)

    glEnd()
    glutSwapBuffers()


def run():

    min_x, max_x, min_y, max_y = None, None, None, None

    for uid in coord.samples:
        feats = coord.clusterer.get_estimator_features(uid).as_list()

        for y, x in enumerate(feats):
            if min_x is None or x < min_x:
                min_x = x
            if max_x is None or x > max_x:
                max_x = x
            if min_y is None or y < min_y:
                min_y = y
            if max_y is None or y > max_y:
                max_y = y

    glutInit([])
    glutCreateWindow('foobar')
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA)

    glPointSize(3)
    glEnable(GL_POINT_SMOOTH)

    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    glClearColor(0, 0, 0, 1)
    glShadeModel(GL_SMOOTH)
    glEnable(GL_CULL_FACE)

    glutDisplayFunc(draw)

    glMatrixMode(GL_PROJECTION)

    x_delta = max_x - min_x
    y_delta = max_y - min_y

    min_x -= x_delta * 0.05
    max_x += x_delta * 0.05
    min_y -= y_delta * 0.05
    max_y += y_delta * 0.05

    print min_x, max_x, min_y, max_y
    glOrtho(min_x, max_x, min_y, max_y, 1, -1)

    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    
    glutMainLoop()
    return

run()

