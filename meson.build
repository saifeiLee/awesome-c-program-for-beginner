project('awesome-c-program-for-beginner', 'c')

c_compiler_flags = [
    '-lm',
    '-g',
    ]
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)
add_global_arguments(c_compiler_flags, language: 'c')

subdir('tiny-macro-processor')