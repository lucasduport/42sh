import unittest
import subprocess
import yaml
import os
import argparse
from termcolor import colored

# Constants
BINARY = "../src/42sh"
REF = ""

class TestShellScript(unittest.TestCase):
    def __init__(self):
        super(TestShellScript, self).__init__()
        self.successful_tests = 0
        self.failed_tests = 0

    def setUp(self):
        # Set up any common resources or configurations needed for tests
        pass

    def tearDown(self):
        # Clean up after each test
        pass

    def run_command(self, command, input_type):
        if input_type == "file":
            result = subprocess.run(f"{BINARY} < {command}", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            return result
        elif input_type == "cmd_arg":
            result = subprocess.run(f"{BINARY} -c \"{command}\"", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            return result
        elif input_type == "stdin":
            result = subprocess.run(f"{BINARY}", shell=True, input=command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            return result

    def run_test(self, test_case, input_type, verbose=False):
        category = test_case.get("category", "")
        sub_category = test_case.get("sub-category", "")
        command = test_case.get("command", "")

        # Calculate expected output and return code using binary
        binary_result = self.run_command(command, input_type)

        expected_result = subprocess.run(f"/bin/bash --posix", shell=True, input=command ,stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if binary_result.stdout != expected_result.stdout or binary_result.stderr != expected_result.stderr or binary_result.returncode != expected_result.returncode:
            print(colored(f"Test failed: {category} - {sub_category} - {input_type}: {command}", 'red'))
            self.failed_tests += 1
        else:
            print(colored(f"Test passed: {category} - {sub_category} - {input_type}: {command}", 'green'))
            self.successful_tests += 1

        if verbose:
            if binary_result.stdout != expected_result.stdout:
                print(colored(f"Expected STDOUT: {expected_result.stdout}", 'green'))
                print(colored(f"Actual STDOUT: {binary_result.stdout}", 'yellow'))
            if binary_result.stderr != expected_result.stderr:
                print(colored(f"Expected STDERR: {expected_result.stderr}", 'green'))
                print(colored(f"Actual STDERR: {binary_result.stderr}", 'yellow'))
            if binary_result.returncode != expected_result.returncode:
                print(colored(f"Expected Return Code: {expected_result.returncode}", 'green'))
                print(colored(f"Actual Return Code: {binary_result.returncode}", 'yellow'))

        print()
        return
        


    def create_test_from_case(self, test_case, input_type, verbose=False):
        return lambda: self.run_test(test_case, input_type, verbose=verbose), ()

    def test_from_file(self, file_path, verbose=False):
        with open(file_path, 'r') as file:
            test_data = yaml.safe_load(file)

        for category_data in test_data:
            category = category_data.get("category", "")
            sub_category = category_data.get("sub-category", "")
            tests = category_data.get("tests", [])

            if verbose:
                print(colored(f"\nRunning tests for: {category} - {sub_category}\n", 'cyan'))

            for test_case in tests:
                for input_type in ["file", "cmd_arg", "stdin"]:
                    test_function, test_args = self.create_test_from_case(test_case, input_type, verbose=verbose)
                    test_function(*test_args, input_type)

    def run_category_tests(self, category_data, verbose=False):
        category = category_data.get("category", "")
        sub_category = category_data.get("sub-category", "")
        tests = category_data.get("tests", [])

        if verbose:
            print(colored(f"\nRunning tests for: {category} - {sub_category}\n", 'cyan'))

        for test_case in tests:
            for input_type in ["cmd_arg"]:
                test_function, test_args = self.create_test_from_case(test_case, input_type, verbose=verbose)
                test_function(*test_args)

        print(colored("\n***************************************************", 'blue'))
        # Print category summary
        print(colored(f"Category summary: {category} - {sub_category}", 'yellow'))
        print(f"Total tests: {self.successful_tests + self.failed_tests}")
        print(f"Success: {self.successful_tests}")
        print(f"Failures: {self.failed_tests}")

        if self.successful_tests + self.failed_tests > 0:
            success_percentage = (self.successful_tests * 100) / (self.successful_tests + self.failed_tests)
            print(f"Category Success Rate: {success_percentage:.2f}%")

        if self.failed_tests == 0:
            print(colored("All category tests passed!", 'green'))
        else:
            print(colored("Some category tests failed.", 'red'))

        print(colored("***************************************************", 'blue'))

class TestRunner:
    def __init__(self, test_files, verbose=False):
        self.test_files = test_files
        self.verbose = verbose

    def run_tests(self):
        for test_file in self.test_files:
            with open(test_file, 'r') as file:
                test_data = yaml.safe_load(file)
            
            for category_data in test_data:
                test_shell_script = TestShellScript()
                test_shell_script.run_category_tests(category_data, verbose=self.verbose)

def load_test_files(directory):
    test_files = [f for f in os.listdir(directory) if f.endswith('.yaml') or f.endswith('.yml')]
    return [os.path.join(directory, file) for file in test_files]

def main():
    parser = argparse.ArgumentParser(description='Run shell script tests from YAML files.')
    parser.add_argument('test_files_directory', type=str, help='Directory containing test YAML files')
    parser.add_argument('--verbose', action='store_true', help='Enable verbose mode')
    args = parser.parse_args()

    test_files = load_test_files(args.test_files_directory)

    test_runner = TestRunner(test_files, verbose=args.verbose)
    test_runner.run_tests()

if __name__ == '__main__':
    main()
