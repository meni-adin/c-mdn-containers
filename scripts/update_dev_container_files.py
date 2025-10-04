import os
import utils

IGNORE = ['build', '.git',]

for item in os.listdir(utils.PROJECT_DIR):
    if item in IGNORE:
        continue

    print(f'Updating {item}')

    command = f'docker exec {utils.REPO_NAME}-cont rm -rf /root/{utils.REPO_NAME}/{item}'
    utils.run_command(command, shell=True, check=True)

    item_full_path = utils.PROJECT_DIR / item
    command = f'docker cp {item_full_path} {utils.REPO_NAME}-cont:/root/{utils.REPO_NAME}/'
    utils.run_command(command, shell=True, check=True)
