
import cluster.featurization
import cluster.normalization
import cluster.estimation
import cluster.feature_transform

import bootstrap

import urllib
import random
import sys
import re
import os

from OpenGL.GLUT import * 
from OpenGL.GL import * 
from OpenGL.GLU import *

if len(sys.argv) != 2:
    print "Usage: %s /path/to/dataset" % sys.argv[0]
    sys.exit(-1)

cmem_empty = {'pos': 0, 'neg': 0}

def iterate_docs(relative_path, klass):

    subdoc_re = re.compile(r'<dmoz_subdoc>(.+?)</dmoz_subdoc>', re.DOTALL)

    for ind, m in enumerate(
        subdoc_re.finditer(
            open(os.path.join(sys.argv[1], relative_path)
                ).read())):
        yield '%s_%04d' % (klass, ind), m.group(1)

inj = bootstrap.bootstrap()
intern_tbl = inj.get_instance(cluster.featurization.InternTable)
normalizer = inj.get_instance(cluster.normalization.Normalizer)
featurizer = inj.get_instance(cluster.featurization.basic_featurize.TfIdfFeaturizer)


### DF
for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
    featurizer.accumulate_doc_frequency(normalizer.normalize(doc_content))
for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
    featurizer.accumulate_doc_frequency(normalizer.normalize(doc_content))

#feat_transform = inj.get_instance(cluster.feature_transform.PCAProjIGainCutoffTransform)

#feat_transform = inj.get_instance(cluster.feature_transform.PCAIGainCutoffTransform)
feat_transform = inj.get_instance(cluster.feature_transform.PCAProjTransform)

#### Feature Selection
train_feats, train_probs = [], []
for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
    train_feats.append( featurizer.featurize( normalizer.normalize(doc_content)))
    train_probs.append( [1, 0])

for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
    train_feats.append( featurizer.featurize( normalizer.normalize(doc_content)))
    train_probs.append( [0, 1])

print "FEATURE SELECTION: "
print feat_transform.train_transform(train_feats, train_probs)

d0, d1 = 0, 1
#### Draw

def draw():

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    glBegin(GL_POINTS)

    glColor(0, 1, 0, 0.2)
    for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
        doc_feats = featurizer.featurize( normalizer.normalize(doc_content))
        doc_feats = feat_transform.transform(doc_feats)

        pos = doc_feats.as_list()
        for ind, p in enumerate(pos):
            glVertex3f(p, ind + 1, 0)

        #print pos
#        glVertex3f(pos[d0], pos[d1], 0)

    glColor(1, 0, 0, 0.2)
    for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
        doc_feats = featurizer.featurize( normalizer.normalize(doc_content))
        doc_feats = feat_transform.transform(doc_feats)

        pos = doc_feats.as_list()
        for ind, p in enumerate(pos):
            glVertex3f(p, ind + 1, 0)

        #print pos
#        glVertex3f(pos[d0], pos[d1], 0)

    glEnd()
    glutSwapBuffers()

def run(name):

    glutInit([])
    glutCreateWindow('foobar')
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA )

    glPointSize(3)
    glEnable(GL_POINT_SMOOTH)

    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    glClearColor(0, 0, 0, 1)
    glShadeModel(GL_SMOOTH)
    glEnable(GL_CULL_FACE)
#    glEnable(GL_DEPTH_TEST)

    #print "entropy: %f" % clus.expect_and_maximize()
    glutDisplayFunc(draw)
    #glutKeyboardFunc(keyboard)

    glMatrixMode(GL_PROJECTION)
    glOrtho(-0.25, 0.25, -0.5, 50.5, 1, -1)

    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    
    glutMainLoop()
    return

run('display')


