import utils

command = f'docker build -t {utils.REPO_NAME}-img {utils.PROJECT_DIR}'
utils.run_command(command, shell=True, check=True)
