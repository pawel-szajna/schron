import subprocess, os

project = 'Schron'
copyright = '2023 Schron development team'
author = 'Schron development team'

extensions = ['breathe', 'sphinxcontrib.luadomain']
breathe_projects = {}
breathe_default_project = 'schron'

templates_path = ['templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme = 'furo'
html_static_path = ['static']

if os.environ.get('READTHEDOCS', None) == 'True':
    with open('Doxyfile', 'r') as doxyfile_src:
        contents = doxyfile_src.read()
    contents = contents.replace('@DOXYGEN_INPUT@', '../src')
    contents = contents.replace('@DOXYGEN_OUTPUT@', 'build')
    with open('Doxyfile', 'w') as doxyfile_tgt:
        doxyfile_tgt.write(contents)
    subprocess.call('doxygen', shell=True)
    breathe_projects['schron'] = 'build/xml'

