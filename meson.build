project('cloud-ar', 'cpp')

executable('cloud-ar', 'src/app.cpp', 
    cpp_args : '-g',
    link_args : '-static-libstdc++',
    dependencies : [
        dependency('gtkmm-4.0'),
        dependency('opencv4')
    ],
)