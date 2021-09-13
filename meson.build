project('remote_ev', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

my_src = files([
  'src/main.cpp'
])

libevdev = dependency('libevdev')

exe = executable('remote_ev',
  sources: my_src,
  dependencies: [libevdev],
  install : false)

test('basic', exe)
