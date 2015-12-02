#! /bin/bash

root_dir=$(cd `dirname $0`/..; pwd)
pushd $root_dir > /dev/null

test_cases=(
    'add'
)

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# Compile test/lib/lib.o
pushd test/lib > /dev/null
make
popd > /dev/null  # test/lib
echo

# Prepare working directories
rm -rf test/test_cases.cstr
rm -rf test/test_cases.gcc
mkdir test/test_cases.cstr
mkdir test/test_cases.gcc

echo "running integeration tests (test/test_cases):"

for t in ${test_cases[@]}
do
    echo -n "  ${t}.c ..."

    (
        # If any stage of the compilation fails, this will cause the script to
        # break out of the test.
        set -e

        # Compile with gcc.
        gcc -S test/test_cases/${t}.c -o test/test_cases.gcc/${t}.s
        gcc test/test_cases.gcc/${t}.s test/lib/lib.o -o test/test_cases.gcc/${t}

        # Compile with cstr.
        cpp test/test_cases/${t}.c | bin/compiler > test/test_cases.cstr/${t}.ll
        llc test/test_cases.cstr/${t}.ll -o test/test_cases.cstr/${t}.s
        gcc test/test_cases.cstr/${t}.s test/lib/lib.o -o test/test_cases.cstr/${t}

        # Run gcc version.
        test/test_cases.gcc/${t} > test/test_cases.gcc/${t}.stdout 2> test/test_cases.gcc/${t}.stderr

        # Run cstr version.
        test/test_cases.cstr/${t} > test/test_cases.cstr/${t}.stdout 2> test/test_cases.cstr/${t}.stderr
    )

    diff test/test_cases.gcc/${t}.stdout test/test_cases.cstr/${t}.stdout > /dev/null
    if [ "$?" == "0" ]
    then
        diff test/test_cases.gcc/${t}.stderr test/test_cases.cstr/${t}.stderr > /dev/null
        if [ "$?" == "0" ]
        then
            printf " ${GREEN}GOOD${NC}\n"
        else
            printf " ${RED}FAILED${NC} - stderr does not match\n"
        fi
    else
        printf " ${RED}FAILED${NC} - stdout does not match\n"
    fi
done

popd > /dev/null  # $root_dir
