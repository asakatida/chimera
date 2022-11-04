from asyncio import run
from os import environ, uname
from sys import stderr
from venv import venv

from asyncio_cmd import ProcessError, cmd


async def main() -> None:
    environ["PREBUILD_CHECK"] = environ.get("GITPOD_WORKSPACE_URL", "")

    await cmd("git", "submodule", "update", "--init")

    if uname().sysname == "Darwin":
        await cmd("brew", "install", "python3")
        environ["PATH"] = environ["PATH"] + ":" + environ["HOME"] + "/.pyenv/shims"
        await venv("python3")


if __name__ == "__main__":
    try:
        run(main())
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)

# case "$(uname)" in
#   Darwin )
#     python_bin="$(command -v python3)"
#     python3 tools/venv.py "${python_bin}"
#     env/bin/ansible-playbook tools/boot.yml
#     python_bin="$(command -v python3.13)"
#     python3 tools/venv.py "${python_bin}"
#     export PATH="${PWD}/env/bin:${PATH}"
#     ;;
#   Linux )
#     if command -v apt; then
#       /opt/virtualenv/bin/python tools/before.py
#       export PATH="/opt/virtualenv/bin:${PATH}"
#     else
#       echo 'No apt found, failed installation'
#       exit
#     fi
#     ;;
#   * )
#     uname
#     echo 'unknown os, failed installation'
#     exit
#     ;;
# esac

# cmakelint

# chimera_path="${PWD}/stdlib:$(python3 tools/chimera_path.py)"

# CXXFLAGS="$(echo "${CXXFLAGS}" | xargs)"
# export CC="${CC:-clang}"
# export CXX="${CXX:-clang++}" CXXFLAGS="${CXXFLAGS} -DCHIMERA_PATH=${chimera_path}"

# export LLVM_PROFILE_FILE="${PWD}/build/coverage/llvm-profile.%c%p.profraw"

# env \
#   CMAKE_BUILD_TYPE=Debug \
#   python3 tools/cmake.py "${PWD}/build/debug" -Wdev -Werror=dev

# env \
#   CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-MinSizeRel}" \
#   CXXFLAGS="${CXXFLAGS} -DNDEBUG" \
#   python3 tools/cmake.py "${PWD}/build/release"

# python3 tools/lint.py

# python3 tools/supported_distros.py

# ansible-playbook tools/docker.yml

# python3 tools/ninja.py build/debug
# python3 tools/ninja.py build/release
