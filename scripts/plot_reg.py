
import vinz.featurization
import vinz.normalization
import vinz.estimation
import vinz.cluster
import vinz.feature_transform

import restful
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
    print "Usage: %s <clustering-name>" % sys.argv[0]
    sys.exit(-1)


####### STUFF
inj = bootstrap.bootstrap()
intern_tbl = inj.get_instance(vinz.featurization.InternTable)
normalizer = inj.get_instance(vinz.normalization.Normalizer)
featurizer = inj.get_instance(vinz.featurization.basic_featurize.TfIdfFeaturizer)
clusterer  = inj.get_instance(vinz.cluster.SparseGaussEmClusterer)

serv = restful.Service('http://localhost:8080')

# Query clustering
clustering = serv.get('/clustering.json', {'name': sys.argv[1]})
assert clustering, "No such clustering: %s" % sys.argv[1]
clustering = clustering[0]

# Query clusters of the clustering
clusters = serv.get('/cluster.json', {'clustering_id': clustering['id']})

# Query documents participating in this clustering
clustering_docs = serv.get('/clustering_document.json',
    {'clustering_id': clustering['id']})

# Load documents & document <=> cluster relations
step, all_documents = 50, []
for i in xrange(0, len(clustering_docs), step):

    all_documents.extend( serv.get('/document.json',
        [('id', j['document_id']) for j in clustering_docs[i:i+step]]))


estimators = {}
for cluster in clusters:
    estimators[cluster['name']] = inj.get_instance(vinz.estimation.Estimator)

#feat_transform = inj.get_instance(vinz.feature_transform.PCAProjIGainCutoffTransform)
#feat_transform = inj.get_instance(vinz.feature_transform.PCAIGainCutoffTransform)
feat_transform = inj.get_instance(vinz.feature_transform.PCAProjTransform)


for doc in all_documents:
    doc['norm_content'] = normalizer.normalize(doc['content'])
    featurizer.accumulate_doc_frequency( doc['norm_content'])

train_feats, train_probs = [], []


filtered_docs = []
for doc in all_documents:
    doc['features'] = featurizer.featurize( doc['norm_content'])

    seen = False
    for o_doc in filtered_docs:
        p = doc['features'].inner_product(o_doc['features'])
        assert p <= 1.0000001
        if p > 0.85:
            seen = True
            break

    if not seen:
        filtered_docs.append(doc)

print "%d filtered docs (of %d)" % (len(filtered_docs), len(all_documents))

for doc in filtered_docs[:]:
    train_feats.append( doc['features'])
    train_probs.append( [0.5, 0.5])

print "FEATURE SELECTION: "
print feat_transform.train_transform(train_feats, train_probs)

#### Draw

def draw():

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    glBegin(GL_POINTS)

    glColor(0, 1, 0, 0.2)
    for doc in filtered_docs:
        doc_feats = feat_transform.transform(doc['features'])

        pos = doc_feats.as_list()
        for ind, p in enumerate(pos):
            glVertex3f(p, ind + 1, 0)

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

    glutDisplayFunc(draw)
    #glutKeyboardFunc(keyboard)

    glMatrixMode(GL_PROJECTION)
    glOrtho(-0.25, 0.25, -0.5, 50.5, 1, -1)

    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    
    glutMainLoop()
    return

run('display')

