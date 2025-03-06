#!/bin/bash

pyenv global 3.9
python setup.py bdist_wheel

pyenv global 3.10
python setup.py bdist_wheel

pyenv global 3.11
python setup.py bdist_wheel

pyenv global 3.12
python setup.py bdist_wheel

pyenv global 3.13
python setup.py bdist_wheel
