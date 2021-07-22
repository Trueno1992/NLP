# file: setup.py
from distutils.core import setup, Extension

module1 = Extension('LD',
                    define_macros = [('MAJOR_VERSION', '1'), ('MINOR_VERSION', '0')],
                    #include_dirs = ['./include'],
                    #library_dirs = ['./src'],
                    #libraries = ['StringUtils.cpp'],
                    sources = ['edit.cpp'])

setup(name = 'yw_ld',
      version = '1.0.1',
      author = 'wulangzhou',
      author_email = '',
      url = 'https://docs.python.org/extending/building',
      description = 'is edit distance demo',
      ext_modules= [module1])
