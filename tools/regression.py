# case $# in
#   2 )
#     find "$2" -type f -print0 | \
#       python "$(dirname "$0")/xargs.py" -- \
#       "$1" \
#       -detect_leaks=0 \
#       -use_value_profile=1 \
#       -max_total_time=1200 \
#       >"/tmp/regression-$(basename "$1").logs" 2>&1
#     ;;
#   * )
#     echo "Usage: $0 <fuzzer> <corpus-dir>" >&2
#     exit 1
#     ;;
# esac
