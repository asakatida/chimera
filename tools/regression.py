from asyncio import run
from sys import argv, stderr

from asyncio_cmd import ProcessError
from corpus_utils import regression

if __name__ == "__main__":
    try:
        run(regression(*argv[1:]))
    except ProcessError as error:
        error.exit()
    except KeyboardInterrupt:
        print("KeyboardInterrupt", file=stderr)
