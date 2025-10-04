import shutil
import utils

SOURCE = utils.PROJECT_DIR/'.gitignore'
DESTINATION = utils.PROJECT_DIR/'.dockerignore'
shutil.copy(SOURCE, DESTINATION)
