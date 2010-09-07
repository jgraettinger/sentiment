import ez_setup
ez_setup.use_setuptools()

import os
import setuptools

def enum_files(base_dir, extension):
    return [os.path.join(base_dir, i) for i in \
        os.listdir(base_dir) if i.endswith(extension)]

extensions = [

    # cluster._cluster
    setuptools.extension.Extension(
        name = 'cluster._cluster',
        sources = enum_files('cluster', '.cpp'),
        include_dirs = ['.', '../include/'],
        libraries = ['boost_python', 'armadillo'],
    ),

    # cluster._common_conversions
    setuptools.extension.Extension(
        name = 'cluster._common_conversions',
        sources = ['cluster/_common_conversions.cpp'],
        include_dirs = ['.', '../include/'],
        libraries = ['boost_python'],
    ),

    # cluster.features._features
    setuptools.extension.Extension(
        name = 'cluster.features._features',
        sources = enum_files('cluster/features', '.cpp'),
        include_dirs = ['.', '../include/'],
        libraries = ['boost_python'],
    ),

    # cluster.feature_transform._feature_transform
    setuptools.extension.Extension(
        name = 'cluster.feature_transform._feature_transform',
        sources = enum_files('cluster/feature_transform', '.cpp'),
        include_dirs = ['.', '../include/'],
        libraries = ['boost_python', 'armadillo'],
    ),

    # cluster.estimation._estimation
    setuptools.extension.Extension(
        name = 'cluster.estimation._estimation',
        sources = enum_files('cluster/estimation', '.cpp'),
        include_dirs = ['.', '../include/'],
        libraries = ['boost_python', 'armadillo'],
    ),
]

setuptools.setup(

    name = 'cluster',
    version = '0.1',
    packages = ['cluster'],

    install_requires = [
        'nose',
    ],

    ext_modules = extensions,
)


