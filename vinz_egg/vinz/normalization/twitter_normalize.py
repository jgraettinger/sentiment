
import re

class TwitterNormalizer(object):

    def __init__(self):
        split_chars = """\r\n\t ~`!@#$%^&*()_-+={[}]|\:;"'<,>.?/"""
        self.re_split = re.compile('\\' + '|\\'.join(split_chars))

    def normalize(self, text):
        text = text.replace('#', '')
        text = text.replace('@', '')
        text = ' '.join(t for t in text.split() if not t.startswith('http'))
        text = ' '.join(t for t in self.re_split.split(text.lower()) if t)
        return text

