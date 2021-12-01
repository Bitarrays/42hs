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
  include_directories('./src/lexer'),
  include_directories('./src/parser'),
  include_directories('./src/shell_input'),
  include_directories('./src/ast_evaluation'),
  include_directories('./src/builtins'),
  include_directories('./src/variable'),
  include_directories('./src/'),
]

common = [
  'src/lexer/lexer.c',
  'src/lexer/lexer_print.c',
  'src/lexer/ionumbers.c',
  'src/lexer/create_token.c',
  'src/lexer/keywords.c',
  'src/lexer/split_input.c',
  'src/parser/parser.c',
  'src/parser/parser_high_level.c',
  'src/parser/parser_conditions.c',
  'src/parser/parser_commands.c',
  'src/parser/parser_loops.c',
  'src/parser/ast.c',
  'src/ast_evaluation/pretty_print.c',
  'src/ast_evaluation/ast_evaluation.c',
  'src/ast_evaluation/builtin_exec.c',
  'src/ast_evaluation/inner_exec.c',
  'src/ast_evaluation/prog_exec.c',
  'src/shell_input/shell_input.c',
  'src/builtins/builtins.c',
  'src/variable/var_list.c',
  'src/builtins/echo.c',
  'src/builtins/cd.c',
]

token_printer = executable(
    # name of the output executable
    '42sh',
    # source files for the executable
    common + [ 'src/42sh.c' ],
    # these are passed as -I
    include_directories: incdirs
)
