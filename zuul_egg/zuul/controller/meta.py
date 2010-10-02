
import clustering
import document
import clustering_document
import cluster
import cluster_document
import secure_token
import static_resource_tree

import zuul.wsgi_route
import getty

class DispatchMapping(object):

    @getty.requires(dispatcher = zuul.wsgi_route.Dispatcher)
    def __init__(self, dispatcher):

        self.dispatcher = dispatcher

        clustering.bind_actions(dispatcher)
        document.bind_actions(dispatcher)
        clustering_document.bind_actions(dispatcher)
        cluster.bind_actions(dispatcher)
        cluster_document.bind_actions(dispatcher)
        secure_token.bind_actions(dispatcher)
        static_resource_tree.bind_actions(dispatcher)
        return

