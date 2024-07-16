from setuptools import setup, Extension
import os

os.environ["CC"] = "gcc-14"
ext = Extension(
    'c_extension',
    sources=['/Users/yuenze/Study_Local/funpar/C_project/c_extension.c'],
    extra_compile_args=['-Ofast', '-march=native', '-funroll-loops', '-fopenmp'],
    # extra_compile_args=['-fopenmp'],
    extra_link_args=['-lgomp'])

setup(name='c_extension',
      version='1.0',
      description='This is a demo package',
      ext_modules=[ext])