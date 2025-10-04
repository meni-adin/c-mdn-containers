from pathlib import Path
import os
import platform
import subprocess
import sys

PROJECT_DIR = Path(__file__).parent.parent.resolve()
SCRIPTS_DIR = PROJECT_DIR/'scripts'
BUILD_TOP_DIR = PROJECT_DIR/'build'
CONFIGURATIONS_DIR = PROJECT_DIR/'configurations'
PYTHON_EXECUTABLE = sys.executable
REPO_NAME = PROJECT_DIR.name

ANSI_ESC = '\033'
ANSI_PARAM_BEGIN = '['
ANSI_PARAM_END = 'm'
COLOR_PREFIX = ANSI_ESC + ANSI_PARAM_BEGIN
COLOR_SUFFIX = ANSI_PARAM_END
COLOR_RESET = COLOR_PREFIX + '0' + COLOR_SUFFIX
COLOR_RED = COLOR_PREFIX + '31' + COLOR_SUFFIX
COLOR_YELLOW = COLOR_PREFIX + '33' + COLOR_SUFFIX
COLOR_GREEN = COLOR_PREFIX + '32' + COLOR_SUFFIX

def program_available(program):
    command = f'{program} --version'
    result = subprocess.run(command, shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return result.returncode == 0

def run_command(command, **kwargs):
    print(f'Running command: {command}')
    result = subprocess.run(command, **kwargs)
    return result

def running_on_github_actions():
    return os.getenv('GITHUB_ACTIONS') == 'true'

def running_on_windows():
    return platform.system() == 'Windows'

def running_on_linux():
    return platform.system() == 'Linux'

def running_on_macos():
    return platform.system() == 'Darwin'

def running_on_unix():
    return os.name() == 'posix'

def colored_print(*args, color=COLOR_RESET, **kwargs):
    text = ' '.join(map(str, args))  # Mimic the way `print` joins arguments
    print(f'{color}{text}{COLOR_RESET}', flush=True, **kwargs)
