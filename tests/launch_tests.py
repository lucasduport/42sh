import unittest
import subprocess
import yaml
import os
import argparse
from termcolor import colored

# Constants
BINARY = "../src/42sh"
REF = "/usr/bin/env bash --posix"

class TestShellScript(unittest.TestCase):
    def __init__(self):
        super(TestShellScript, self).__init__()
        self.results = {}  # Dictionary to store test results
        self.total_success = 0
        self.total_fail = 0

    def setUp(self):
        # Set up any common resources or configurations needed for tests
        pass

    def tearDown(self):
        # Clean up after each test
        pass

    def run_command(self, binary, command, input_type):
        # Create a temporary file to store the command
        temp_file_path = ".tmp_tst"
        with open(temp_file_path, 'w') as temp_file:
            temp_file.write(command)

        if input_type == "file_b_n":
            with open(temp_file_path, 'a') as temp_file:
                temp_file.write("\n")
            
        if input_type == "file":
            result = subprocess.run(f"{binary} {temp_file_path}", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=3)
        elif input_type == "cmd_arg":
            result = subprocess.run(f"{binary} -c \"{command}\"", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True,  timeout=3)
        elif input_type == "stdin":
            result = subprocess.run(f"{binary} < {temp_file_path}", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True,  timeout=3)
        elif input_type == "file_b_n":
            result = subprocess.run(f"{binary} {temp_file_path} -n", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True,  timeout=3)
        
        # Delete the temporary file after execution
        os.remove(temp_file_path)
        return result

    def run_test(self, test_case, yaml_data, input_type, verbose=False):
        category = yaml_data.get("category", "")
        sub_category = yaml_data.get("sub-category", "")
        command = test_case.get("command", "")

        # Run our 42sh binary
        binary_result = self.run_command(BINARY, command, input_type)
        # Run the reference posix shell
        expected_result = self.run_command(REF, command, input_type)

        if binary_result.stdout != expected_result.stdout \
            or (len(binary_result.stderr) > 0) != (len(expected_result.stderr) > 0) \
                or binary_result.returncode != expected_result.returncode:
            print(colored(f"✘ {category} - {sub_category} - {input_type}: {command}", 'red'))
            self.results[category][sub_category]["fail"] += 1
        else:
            #if verbose:
                #print(colored(f"✔ {category} - {sub_category} - {input_type}: {command}", 'green'))
            self.results[category][sub_category]["success"] += 1

        if verbose:
            if binary_result.stdout != expected_result.stdout:
                print(colored(f"\tExpected (stdout): {expected_result.stdout}", 'green'))
                print(colored(f"\tActual (stdout): {binary_result.stdout}", 'yellow'))
            if (len(binary_result.stderr) > 0) != (len(expected_result.stderr) > 0):
                print(colored(f"\tExpected (stderr): {expected_result.stderr}", 'green'))
                print(colored(f"\tActual stderr: {binary_result.stderr}", 'yellow'))
            if binary_result.returncode != expected_result.returncode:
                print(colored(f"\tExpected (exit code): {expected_result.returncode}", 'green'))
                print(colored(f"\tActual (exit code): {binary_result.returncode}", 'yellow'))
        return

    def create_test_from_case(self, test_case, yaml_data, input_type, verbose=False):
        return lambda: self.run_test(test_case, yaml_data, input_type, verbose=verbose), ()

    def run_category_tests(self, yaml_data, verbose=False):
        category = yaml_data.get("category", "")
        sub_category = yaml_data.get("sub-category", "")
        tests = yaml_data.get("tests", [])

        if category not in self.results:
            self.results[category] = {}

        if sub_category not in self.results[category]:
            self.results[category][sub_category] = {"success": 0, "fail": 0}

        for test_case in tests:
            for input_type in ["cmd_arg", "file", "stdin", "file_b_n"]:
                test_function, test_args = self.create_test_from_case(test_case, yaml_data, input_type, verbose=verbose)
                test_function(*test_args)

    def print_category_summary(self, category):
        print(colored(f"******** {category} summary ********", 'cyan'))
        category_success = 0
        category_fail = 0
        for sub_category in self.results[category]:
            print(colored(f"\t--- {sub_category} summary ---", 'cyan'))
            sub_success = self.results[category][sub_category]["success"]
            sub_fail = self.results[category][sub_category]["fail"]
            print(colored(f"\t✔: {sub_success}", 'green'), colored(f"✘: {sub_fail}", 'red'))
            if sub_success + sub_fail > 0:
                success_percentage = (sub_success * 100) / (sub_success + sub_fail)
                p_colour = get_print_colour(success_percentage)
                print(colored(f"\tRatio: {success_percentage:.2f}%", p_colour))
            print()
            category_success += sub_success
            category_fail += sub_fail
        print(colored(f"✔: {category_success}", 'green'), colored(f"✘: {category_fail}", 'red'))
        if category_success + category_fail > 0:
            success_percentage = (category_success * 100) / (category_success + category_fail)
            p_colour = get_print_colour(success_percentage)
        print(colored(f"Ratio: {success_percentage:.2f}%", p_colour))
        print()
        self.total_success += category_success
        self.total_fail += category_fail
    
    def print_summary(self):
        for category in self.results:
            self.print_category_summary(category)
        print()
        print(colored(f"************ Total summary ************", 'cyan'))
        print(colored(f"✔: {self.total_success}", 'green'), colored(f"✘: {self.total_fail}", 'red'))
        if self.total_success + self.total_fail > 0:
            success_percentage = (self.total_success * 100) / (self.total_success + self.total_fail)
            p_colour = get_print_colour(success_percentage)
            # Different color depending on success rate
            if success_percentage >= 90:
                p_colour = 'green'
            elif success_percentage >= 60:
                p_colour = 'yellow'
            else:
                p_colour = 'red'
        print(colored(f"Ratio: {success_percentage:.2f}%", p_colour))
        print(colored('*' * 39, 'cyan'))

class TestRunner:
    def __init__(self, test_files, verbose=False):
        self.test_files = test_files
        self.verbose = verbose

    def run_tests(self):
        test_shell_script = TestShellScript()
        for test_file in self.test_files:
            with open(test_file, 'r') as file:
                test_data = yaml.safe_load(file)
            for category_data in test_data:
                test_shell_script.run_category_tests(category_data, verbose=self.verbose)
        test_shell_script.print_summary()

def get_print_colour(success_percentage):
    if success_percentage >= 90:
        return 'green'
    elif success_percentage >= 60:
        return 'yellow'
    else:
        return 'red'

def load_test_files(directory):
    if not os.path.isdir(directory):
        return [directory]
    test_files = [f for f in os.listdir(directory) if f.endswith('.yaml') or f.endswith('.yml')]
    return [os.path.join(directory, file) for file in test_files]

def main():
    parser = argparse.ArgumentParser(description='Run our 42sh tests from YAML files.')
    parser.add_argument('test_files_directory', type=str, help='Directory containing test YAML files')
    parser.add_argument('--verbose', action='store_true', help='Enable verbose mode')
    args = parser.parse_args()

    test_files = load_test_files(args.test_files_directory)

    test_runner = TestRunner(test_files, verbose=args.verbose)
    test_runner.run_tests()

if __name__ == '__main__':
    main()
