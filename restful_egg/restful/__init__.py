
import urllib
import urllib2
import cookielib
import cjson

class ServiceError(RuntimeError): pass

class Service(object):

    def __init__(self, uri, debuglevel = 0):
        self._uri = uri
        self._cjar = cookielib.CookieJar()
        self._path_stack = []
        self._get_stack = []
        self._post_stack = []

        self._client = urllib2.OpenerDirector()
        self._client.add_handler(urllib2.HTTPHandler(debuglevel = debuglevel))
        self._client.add_handler(urllib2.HTTPCookieProcessor(self._cjar))
        return

    def clone(self):
        srv = Service(self._uri)
        srv._cjar = self._cjar
        srv._path_stack = list(self._path_stack)
        srv._get_stack = list(self._get_stack)
        srv._post_stack = list(self._post_stack)
        srv._client = self._client
        return srv

    def push_path(self, path_component):
        self._path_stack.append(path_component)
        return self

    def pop_path(self):
        self._path_stack.pop()
        return self

    def clear_paths(self):
        self._path_stack = []
        return self

    def push_get_param(self, key, value):
        self._get_stack.append((key, value))
        return self

    def pop_get_param(self):
        self._get_stack.pop()
        return self

    def clear_get_params(self):
        self._get_stack = []
        return self

    def push_post_param(self, key, value):
        self._post_stack.append((key, value))
        return self

    def pop_post_param(self):
        self._post_stack.pop()
        return self

    def clear_post_params(self):
        self._post_stack = []
        return self

    def _urlencode(self, plist):
        return urllib.urlencode([
            (unicode(k).encode('utf8'), unicode(v).encode('utf8')) for k, v in plist])

    def _request(self, method, path, get_params, post_params):

        if isinstance(get_params, dict):
            get_params = get_params.items()
        get_params = self._get_stack + list(get_params)

        if isinstance(post_params, dict):
            post_params = post_params.items()
        post_params = self._post_stack + list(post_params)

        if method in ('GET', 'HEAD'):
            post_params = None

        full_path = '%s%s%s' % (self._uri, ''.join(self._path_stack), path)

        if method not in ('GET', 'POST'):
            get_params += [('_method', method)]

        if get_params:
            full_path += '?%s' % self._urlencode(get_params)

        if post_params:
            resp = self._client.open(full_path, self._urlencode(post_params))
        else:
            resp = self._client.open(full_path)

        if resp.code != 200:
            raise ServiceError(resp.read())

        con_type = resp.headers.get('content-type', '').split('; ')

        resp_body = resp.read()

        if 'charset=utf8' in con_type:
            resp_body.decode('utf8')

        if 'application/json' in con_type:
            resp_body = cjson.decode(resp_body)

        return resp_body

    def get(self, path = '', get_params = ()):
        return self._request('GET', path, get_params, ())

    def post(self, post_params = (), path = '', get_params = ()):
        return self._request('POST', path, get_params, post_params)

    def put(self, post_params = (), path = '', get_params = ()):
        return self._request('PUT', path, get_params, post_params)

    def delete(self, path = '', get_params = ()):
        return self._request('DELETE', path, get_params, ())

