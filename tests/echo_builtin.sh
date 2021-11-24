#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
L_CYAN='\033[1;36m'
D_GRAY='\033[1;30m'
NC='\033[0m'

tests=0
success=0

tests() {
    tests=$(($tests + 1))
    mkdir -p tmp
    echo -n "$@" > tmp/test.sh
    cat tmp/test.sh | ./42sh 1>tmp/out.log 2>tmp/err_out.log
    out_res=$?
    cat tmp/test.sh | dash 1>tmp/ref.log 2>tmp/err_ref.log
    ref_res=$?
    diff -u tmp/out.log tmp/ref.log > tmp/diff_output.log
    diff_output=$?
    valgrind ./42sh -c "$@" 2>&1 | grep 'All heap blocks were freed -- no leaks are possible' 1> /dev/null
    valgrind_res=$?
    printf "${L_CYAN}|---- ${NC}\n"
    printf "${L_CYAN}+${NC}    ./42sh ${ORANGE}"
    for arg; do
        printf '%s' "$arg "
    done
    printf "${NC}${D_GRAY}|${NC} Result: "
    if [ ! $valgrind_res -eq 0 ]; then
        printf "${RED}KO${NC} (LEAKS)\n"
    elif [ -s tmp/err_out.log ] && [ ! -s tmp/err_ref.log ]; then
        printf "${RED}KO${NC} (STDERR_NOT_EMPTY)\n"
    elif [ -s tmp/err_ref.log ] && [ ! -s tmp/err_out.log ]; then
        printf "${RED}KO${NC} (STDERR_EMPTY)\n"
    elif [ ! $diff_output -eq 0 ]; then
        mkdir -p tmp/echo_builtin
        cp tmp/diff_output.log tmp/echo_builtin/$tests.log
        printf "${RED}KO${NC} (OUTPUT_ERR)\n"
    else
        printf "${GREEN}OK${NC}\n"
        success=$((success + 1))
    fi

    rm -f tmp/ref.log tmp/out.log tmp/err_out.log tmp/err_ref.log tmp/diff_output.log tmp/test.sh
}

tests echo -n \"salut\"
tests echo -e \"\\nsalut\"

errors=$(($tests - $success))
printf "${L_CYAN}---------${NC}\n"
echo ""
echo ""
printf "Debrief: ${L_CYAN}${tests}${NC} Tests | ${GREEN}${success}${NC} Success | ${RED}${errors}${NC} Errors\n"

exit $errors
