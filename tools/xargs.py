# case "$(uname)" in
#   Darwin )
#     xargs -0 "$@"
#     ;;
#   * )
#     xargs --no-run-if-empty --null "$@"
#     ;;
# esac
