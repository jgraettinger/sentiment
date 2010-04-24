
from cluster import sparse_vectorspace_featurizer
from cluster import dense_vectorspace_featurizer

intern_tab = {}

class BigramFeaturizer(sparse_vectorspace_featurizer):
    
    def featurize(self, doc_sample):
        tokens = doc_sample.content.split()

        feat = {}
        for ind in xrange(len(tokens)):
            un_tok_id = intern_tab.setdefault(tokens[ind], len(intern_tab))
            feat[un_tok_id] = feat.get(un_tok_id, 0) + 1

            if ind == len(tokens) - 1: continue

            bi_tok_id = intern_tab.setdefault(' '.join(tokens[ind:ind+2]), len(intern_tab))
            feat[bi_tok_id] = feat.get(bi_tok_id, 0) + 1
        
        if not feat:
            print doc_sample.uid, doc_sample.content
        return feat 

