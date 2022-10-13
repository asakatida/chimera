# build="$1"
# shift
# cd "$(git rev-parse --show-toplevel || true)"

# env \
#   CMAKE_EXPORT_COMPILE_COMMANDS=YES \
#   cmake \
#   -DBUILD_SHARED_LIBS=OFF \
#   -GNinja \
#   -S . \
#   -B "${build}" \
#   "$@"
