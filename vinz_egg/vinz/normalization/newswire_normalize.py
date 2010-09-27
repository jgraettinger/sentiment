
import re

class NewswireNormalizer(object):

    def __init__(self):
        split_chars = """\r\n\t ~`!@#$%^&*()_-+={[}]|\:;"'<,>.?/"""
        self.re_split = re.compile('\\' + '|\\'.join(split_chars))

    def normalize(self, doc):

        doc = dict(doc)
        doc['content'] = ' '.join(t for t in \
            self.re_split.split(doc['content']) if t)

        doc['title'] = ' '.join(t for t in \
            self.re_split.split(doc['title'].lower()) if t)

        return doc
