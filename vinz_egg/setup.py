import ez_setup
ez_setup.use_setuptools()

import os
import setuptools

def enum_files(base_dir, *extension):
    return [os.path.join(base_dir, i) for i in \
        os.listdir(base_dir) if i.endswith(extension)]

extensions = [

    # vinz._common_conversions
    setuptools.extension.Extension(
        name = 'vinz._common_conversions',
        sources = ['vinz/_common_conversions.cpp'],
        include_dirs = ['vinz', '../include/'],
        libraries = ['boost_python'],
    ),

    # vinz.features._features
    setuptools.extension.Extension(
        name = 'vinz.features._features',
        sources = enum_files('vinz/features', '.cpp'),
        include_dirs = ['vinz', '../include/'],
        libraries = ['boost_python', 'armadillo'],
    ),

    # vinz.feature_transform._feature_transform
    setuptools.extension.Extension(
        name = 'vinz.feature_transform._feature_transform',
        sources = enum_files('vinz/feature_transform', '.cpp'),
        include_dirs = ['vinz', '../include/'],
        libraries = ['boost_python', 'armadillo'],
    ),

    # vinz.estimation._estimation
    setuptools.extension.Extension(
        name = 'vinz.estimation._estimation',
        sources = enum_files('vinz/estimation', '.cpp'),
        include_dirs = ['vinz', '../include/'],
        libraries = ['boost_python', 'armadillo'],
    ),
    
    # vinz.cluster._cluster
    setuptools.extension.Extension(
        name = 'vinz.cluster._cluster',
        sources = enum_files('vinz/cluster', '.cpp'),
        include_dirs = ['vinz', '../include/'],
        libraries = ['boost_python', 'armadillo'],
    ),
]

setuptools.setup(

    name = 'Vinz',
    version = '0.1',
    packages = ['vinz'],

    install_requires = [
        'nose',
    ],

    ext_modules = extensions,
)


