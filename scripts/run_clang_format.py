import utils

CLANG_FORMAT_CFG_PATH = utils.CONFIGURATIONS_DIR/'clang_format.yml'

directories = [
    utils.PROJECT_DIR/'src',
    utils.PROJECT_DIR/'test',
]

c_cpp_files = []
for directory in directories:
    c_cpp_files.extend(directory.rglob("*.c"))
    c_cpp_files.extend(directory.rglob("*.h"))
    c_cpp_files.extend(directory.rglob("*.cpp"))
    c_cpp_files.extend(directory.rglob("*.hpp"))

for file in c_cpp_files:
    print(f'Running clang-format on {file}')
    command = f'clang-format -style=file:{CLANG_FORMAT_CFG_PATH} -i {file}'
    utils.run_command(command, shell=True, check=True)
