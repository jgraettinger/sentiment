import ez_setup
ez_setup.use_setuptools()

import os
import setuptools

setuptools.setup(

    name = 'Zuul',
    version = '0.1',
    packages = ['zuul'],
    scripts = ['start_zuul.py'],

    install_requires = [
        'Paste',
        'Routes',
        'WebOb',
        'Mako',
        'SqlAlchemy',
        'FormEncode',
        'Eventlet',
        'Greenlet',
        'WebHelpers',
    ],
)


