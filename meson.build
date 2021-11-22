project(
  '42sh',
  'c',
  version : '1.0',
  default_options: [
    'c_std=c99',
    'warning_level=3',
    'werror=true',
  ],
)

cflags = ['-D_GNU_SOURCE', '-D_POSIX_C_SOURCE=200809L']
add_project_arguments(cflags, language: 'c')

incdirs = [
  include_directories('src/lexer'),
  include_directories('src/parser'),
  include_directories('src/shell_input'),
]

common = [
  'src/lexer/lexer.c',
  'src/parser/parser.c',
  'src/shell_input/shell_input.c',
]

token_printer = executable(
    # name of the output executable
    '42sh',
    # source files for the executable
    common + [ 'src/42sh.c' ],
    # these are passed as -I
    include_directories: incdirs
)
