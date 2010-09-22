
import urllib
import re

class BasicNormalizer(object):

    def __init__(self):
        split_chars = """\r\n\t ~`!@#$%^&*()_-+={[}]|\:;"'<,>.?/"""
        self.re_split = re.compile('\\' + '|\\'.join(split_chars))

    def normalize(self, text):

        text = urllib.unquote(text).decode('utf8', 'ignore')
        text = ' '.join(t for t in text.split() if not t.startswith('http') and not t.startswith('@'))
        text = ' '.join(t for t in self.re_split.split(text.lower()) if t and t != 'rt')
        return text


class NoOpNormalizer(object):
    
    def __init__(self):
        return

    def normalize(self, obj):
        return obj

