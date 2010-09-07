import ez_setup
ez_setup.use_setuptools()

import os
import setuptools

setuptools.setup(

    name = 'Zuul',
    version = '0.1',
    packages = ['zuul'],

    install_requires = [
        'Paste',
        'Routes',
        'WebOb',
        'Mako',
        'SqlAlchemy',
        'FormEncode',
        'GEvent',
        'Greenlet',
        'WebHelpers',
    ],
)


