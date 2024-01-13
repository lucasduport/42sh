#!/bin/sh

# FIXME : tests every input types

# Constants
BINARY="../src/42sh"
REF="/bin/bash --posix"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
PURPLE='\033[0;35m'
YELLOW='\033[0;33m'
RESET='\033[0m'

total_tests=0
success_tests=0

DELIMITER="¤"

# Mode
MODE="verbose"

# Function to run a test for a given input type
run_test() {
    description="$1"
    command="$2"

    # Run the test with file input
    #echo $command > .tmp
    #run_test_internal "${description} (File)" ".tmp"
    #rm -f .tmp

    # Run the test with command-line argument input
    run_test_internal "${description} (Cmd Arg)" "-c \"$command\""

    # Run the test with standard input
    #run_test_internal "${description} (Stdin)" "< \"$command\""
}

# Internal function to run the actual test
run_test_internal() {
    description="$1"
    command="$2"

    expected_output=$(eval "$REF $command")
    expected_retcode=$?
    expected_stderr=$(eval "$REF $command" 2>&1 >/dev/null)
    result_binary_stdout=$(eval "$BINARY $command" 2>&1)
    retcode_binary=$?

    if [ "$result_binary_stdout" = "$expected_output" ]; then
        stdout_comparison="${GREEN}✔${RESET}"
    else
        stdout_comparison="${RED}✘${RESET}"
    fi

    if [ "$result_binary_stderr" = "$expected_stderr" ]; then
        stderr_comparison="${GREEN}✔${RESET}"
    else
        stderr_comparison="${RED}✘${RESET}"
    fi

    if [ "$retcode_binary" -eq "$expected_retcode" ]; then
        retcode_comparison="${GREEN}✔${RESET}"
    else
        retcode_comparison="${RED}✘${RESET}"
    fi

    if [ "$result_binary_stdout" = "$expected_output" ] && [ "$result_binary_stderr" = "$expected_stderr" ] && [ "$retcode_binary" -eq "$expected_retcode" ]; then
        success_tests=$((success_tests + 1))
    fi

    # Display result only in verbose mode
    if [ "$MODE" = "verbose" ]; then
        printf "${stdout_comparison}  ${stderr_comparison}  ${retcode_comparison}  ${description}\n"

        # Display details in case of failure
        if [ "$stdout_comparison" = "${RED}✘${RESET}" ] || [ "$stderr_comparison" = "${RED}✘${RESET}" ] || [ "$retcode_comparison" = "${RED}✘${RESET}" ]; then
            if [ "$result_binary_stdout" != "$expected_output" ]; then
                printf "${YELLOW}  Expected Output:${RESET}\n$expected_output\n"
                printf "${YELLOW}  Got Output:${RESET}\n$result_binary_stdout\n"
            fi

            if [ "$result_binary_stderr" != "$expected_stderr" ]; then
                printf "${YELLOW}  Expected STDERR:${RESET}\n$expected_stderr\n"
                printf "${YELLOW}  Got STDERR:${RESET}\n$result_binary_stderr\n"
            fi

            # Compare return code
            if [ ! "$retcode_binary" -eq "$expected_retcode" ]; then
                printf "${YELLOW}  Expected Return Code:${RESET} $expected_retcode\n"
                printf "${YELLOW}  Got Return Code:${RESET} $retcode_binary\n"
            fi
        fi
    fi
    printf "\n"
    total_tests=$((total_tests + 1))
}

# Function to run tests from a file
run_tests_from_file() {
    category="$1"
    filename="$2"

    # Saving values before running category tests
    category_tests_before=$total_tests
    category_success_before=$success_tests

    # Use grep to filter lines containing $DELIMITER, and extract the command after it
    current_command=""
    while IFS= read -r line; do
        if echo "$line" | grep -q $DELIMITER; then
            # Extract the command after $DELIMITER and append it to the current command
            current_command=$(echo "$line" | cut -d $DELIMITER -f 2-)
        else
            # Append the line to the current command
            current_command="$current_command$line"
        fi

        # Check if the line is empty or doesn't contain $DELIMITER
        if [ -z "$line" ] || ! echo "$line" | grep -q $DELIMITER; then
            # Run the test with the accumulated command
            run_test "${category_tests}: $current_command" "$current_command"
            current_command=""
        fi
    done < "$filename"

    category_success=$((success_tests - category_success_before))
    category_tests=$((total_tests - category_tests_before))

    # Display category summary
    printf "\n${YELLOW}${category} summary:${RESET}\n"
    printf "Total category tests: ${category_tests}\n"
    printf "Success: ${GREEN}${category_success}${RESET}\n"
    printf "Failures: ${RED}$((category_tests - category_success))${RESET}\n"

    if [ $category_tests -gt 0 ]; then
        category_success_percentage=$((category_success * 100 / category_tests))
        printf "Category Success Rate: ${category_success_percentage}%%\n"
    else
        category_success_percentage=0
        printf "Category Success Rate: N/A (No tests in this category)\n"
    fi

    if [ $category_success_percentage -eq 100 ]; then
        printf "${GREEN}All category tests passed!${RESET}\n"
    else
        printf "${RED}Some category tests failed.${RESET}\n"
    fi
    printf "${YELLOW}***************************************************${RESET}\n\n\n"
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

# Run tests for interpreting files
run_tests_from_file "Builtins" "builtins_tests.sh"

# Print the overall summary
print_summary
