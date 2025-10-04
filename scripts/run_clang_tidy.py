import argparse
import subprocess
import utils

CLANG_TIDY_CFG_PATH = utils.CONFIGURATIONS_DIR/'clang_tidy.yml'

parser = argparse.ArgumentParser(description='Run clang-tidy on source files')
parser.add_argument('-d', '--database', type=str, required=True, help='Compilation-database path')
parser.add_argument('-e', '--error', action='store_true', required=False, help='Treat warnings as errors', default=False)
args = parser.parse_args()

if args.error:
    error_flag = '-warnings-as-errors=*'
else:
    error_flag = ''

directories = [
    utils.PROJECT_DIR/'src',
    utils.PROJECT_DIR/'test',
]

c_cpp_source_files = []
for directory in directories:
    c_cpp_source_files.extend(directory.rglob("*.c"))
    c_cpp_source_files.extend(directory.rglob("*.cpp"))

for file in c_cpp_source_files:
    print(f'Running clang-tidy on {file}')
    command = f'clang-tidy --config-file={CLANG_TIDY_CFG_PATH} {error_flag}  -p {args.database} {file}'
    utils.run_command(command, shell=True, check=True)
