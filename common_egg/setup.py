import ez_setup
ez_setup.use_setuptools()

import os
import setuptools

extensions = [
    setuptools.extension.Extension(
        name = 'common._common_conversions',
        sources = ['common/_common_conversions.cpp'],
        include_dirs = ['.', '../include/'],
        libraries = ['boost_python'],
    ),
]

setuptools.setup(

    name = 'common',
    version = '0.1',
    ext_modules = extensions,
)


