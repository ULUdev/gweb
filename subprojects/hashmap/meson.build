project('hashmap', 'c', license : 'MIT', version : '1.0.0', default_options : ['c_std=c18'])

subdir('src')
include = include_directories('include')
hashmap = library('hashmap', sources, include_directories : include)
hashmap_dep = declare_dependency(include_directories : include, link_with : hashmap)
