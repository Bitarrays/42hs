# 42hs

## Testsuite

First, you need to install the dependencies using pip:

```sh
➜  42sh ✗ pip install -r tests/requirements.txt
```

Make sure to have the 42sh binary, and then execute the tests/moulinette.py file using python3

```sh
➜  42sh ✗ python3 tests/moulinette.py --binary 42sh --tests tests/echo.yml
```
- ```--binary``` is the relative path of the 42sh binary
- ```--tests``` is a list of all yaml files containing tests

A test is formatted like this in a yaml file:

```yml
- name: a name
  input: an input (for example -> echo "Foo!")
```

## Options

In order to debug the program, there are 2 options.

- ```--petty-print``` print the AST before each execution
- ```--verbose``` print some functions output during execution. For now, there is only the lexer output.

:warning: **The order is important**: ```--petty-print``` has to be before ```--verbose```

## Environment

```sh
➜  42-sh-bitarrays git:(main) env -i ./42sh
```

You need to execute the shell with this command, in order to start with an empty environment
