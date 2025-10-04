from datetime import datetime
import utils

command = f'docker build --build-arg CACHEBUST="{datetime.now()}" -t {utils.REPO_NAME}-img {utils.PROJECT_DIR}'
utils.run_command(command, shell=True, check=True)
