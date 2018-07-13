#!/usr/bin/env python3

"""
setup.py file for SWIG example
"""

from distutils.core import setup, Extension


example_module = Extension('_runTimer',
                           sources=['runTimer_wrap.cxx', 'runTimer.cpp'],
                           )

setup (name = 'runTimer',
       version = '0.5',
       author      = "Safronov&Kazantsev&Potapov",
       description = """Wrapper run_timer.exe""",
       ext_modules = [example_module],
       py_modules = ["runTimer"],
       )
