#!/bin/sh

# Constants
BINARY=/bin/bash
REFERENCE=/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
PURPLE='\033[0;35m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Test counters
total_tests=0
success_tests=0
total_category_tests=0
success_category_tests=0

# Function to run a test
run_test() {
    description="$1"
    command="$2"

    # Run the command using both the binary and reference
    result_binary_stdout=$(eval "$BINARY $command" 2>&1)
    retcode_binary=$?

    result_reference_stdout=$(eval "$REFERENCE $command" 2>&1)
    retcode_reference=$?

    # Compare standard output
    if [ "$result_binary_stdout" = "$result_reference_stdout" ]; then
        stdout_comparison="${GREEN}✔${RESET}"
    else
        stdout_comparison="${RED}✘${RESET}"
        printf "${RED}✘ ${description} - STDOUT${RESET}\n"
        printf "${YELLOW}  Expected:${RESET}\n$result_reference_stdout\n"
        printf "${YELLOW}  Got:${RESET}\n$result_binary_stdout\n"
    fi

    # Run the command again to capture standard error
    result_binary_stderr=$(eval "$BINARY $command" 2>&1 >/dev/null)
    result_binary_stderr=$(eval "$BINARY $command" 2>&1)
    
    result_reference_stderr=$(eval "$REFERENCE $command" 2>&1)
    
    # Compare standard error
    if [ "$result_binary_stderr" = "$result_reference_stderr" ]; then
        stderr_comparison="${GREEN}✔${RESET}"
    else
        stderr_comparison="${RED}✘${RESET}"
        printf "${RED}✘ ${description} - STDERR${RESET}\n"
        printf "${YELLOW}  Expected:${RESET}\n$result_reference_stderr\n"
        printf "${YELLOW}  Got:${RESET}\n$result_binary_stderr\n"
    fi

    # Compare return code
    if [ "$retcode_binary" -eq "$retcode_reference" ]; then
        retcode_comparison="${GREEN}✔${RESET}"
        success_tests=$((success_tests + 1))
    else
        retcode_comparison="${RED}✘${RESET}"
        printf "${RED}✘ ${description} - Return Code${RESET}\n"
        printf "${YELLOW}  Expected:${RESET} $retcode_reference\n"
        printf "${YELLOW}  Got:${RESET} $retcode_binary\n"
    fi

    printf "${stdout_comparison}  ${stderr_comparison}  ${retcode_comparison}  ${description}\n"

    total_tests=$((total_tests + 1))
}

# Function to print category header
print_category_header() {
    category="$1"
    printf "\n${YELLOW}***************************************************\n"
    printf "${PURPLE}==============================================\n"
    printf "            Category: ${category}\n"
    printf "==============================================${RESET}\n"
}

# Function to print category summary
print_cat_summary() {
    printf "\n${YELLOW}$1 summary:${RESET}\n"
    printf "Total category tests: ${category_tests}\n"
    printf "Success: ${GREEN}${category_success}${RESET}\n"
    printf "Failures: ${RED}$((category_tests - category_success))${RESET}\n"

    if [ $total_category_tests -gt 0 ]; then
        category_success_percentage=$((category_success * 100 / category_tests))
        printf "Category Success Rate: ${category_success_percentage}%%\n"
    else
        printf "Category Success Rate: N/A (No tests in this category)\n"
    fi

    if [ $category_success_percentage -eq 100 ]; then
        printf "${GREEN}All category tests passed!${RESET}\n"
    else
        printf "${RED}Some category tests failed.${RESET}\n"
    fi
    printf "${YELLOW}***************************************************${RESET}\n\n\n"
}


# Function to run tests from a file
run_tests_from_file() {
    category="$1"
    filename="$2"
    category_tests=0
    category_success=0

    print_category_header "$category"

    while IFS= read -r line; do
        run_test "${category_tests}: ${line}" "$line"
        category_tests=$((category_tests + 1))
        if [ "$stdout_comparison" = "${GREEN}✔${RESET}" ] && [ "$stderr_comparison" = "${GREEN}✔${RESET}" ] && [ "$retcode_comparison" = "${GREEN}✔${RESET}" ]; then
            category_success=$((category_success + 1))
        fi
    done < "$filename"

    success_category_tests=$((success_category_tests + category_success))
    total_category_tests=$((total_category_tests + category_tests))

    print_cat_summary "$category"
}

# Function to print overall test summary
print_summary() {
    printf "\n${PURPLE}Overall Summary:${RESET}\n"
    printf "Total tests: ${total_tests}\n"
    printf "Success: ${GREEN}${success_tests}${RESET}\n"
    printf "Failures: ${RED}$((total_tests - success_tests))${RESET}\n"

    success_percentage=$((success_tests * 100 / total_tests))
    printf "Success Rate: ${success_percentage}%%\n"

    if [ $success_percentage -eq 100 ]; then
        printf "${GREEN}All tests passed!${RESET}\n"
    else
        printf "${RED}Some tests failed.${RESET}\n"
    fi
}

# Run tests
run_tests_from_file "Builtins" "builtins_tests.sh"

# Print the overall summary
print_summary
