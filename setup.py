from setuptools import Extension, setup

import numpy

setup(
    name='np-aligned-alloc',
    description='Numpy aligned memory allocator',
    author='Alex Hudspith',
    version="0.1.dev0",
    url='https://github.com/alexhudspith/np-aligned-alloc',
    ext_modules=[
        Extension(
            name='np_aligned_alloc',
            sources=['src/np_aligned_alloc.c', 'src/np_aligned_alloc_module.c'],
            include_dirs=[numpy.get_include()],
            extra_compile_args=['-Wall', '-Wextra', '-Wold-style-definition', '-std=c18', '-O3', '-flto'],
            define_macros=[
                # ('DEBUG', 1),
            #     ('_FORTIFY_SOURCE', 2),
            ],
            # undef_macros=['NDEBUG'],
            # define_macros=[],
        ),
    ],
    classifiers=[
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: MIT License',
        'Operating System :: POSIX :: Linux',
        'Operating System :: MacOS :: MacOS X',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
    ],
    license='MIT',
    install_requires=['numpy>=1.24.0'],
    extras_require={'test': ['pytest', 'hypothesis']},
    python_requires='>=3.8',
)
