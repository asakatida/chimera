from asyncio import Event, Queue, TimeoutError, gather, wait_for
from os import cpu_count
from sys import stderr
from typing import Awaitable, Iterable, Sequence, TypeVar, Union

T = TypeVar("T")

DEFAULT_LIMIT = max(cpu_count() or 0, 4) // 4


async def worker(
    end: Event,
    queue: Queue[Awaitable[T]],
    output: Queue[T],
) -> None:
    while not (end.is_set() and queue.empty()):
        try:
            coroutine = await wait_for(queue.get(), 0.5)
            await output.put(await coroutine)
            queue.task_done()
        except TimeoutError:
            pass


async def producer(
    end: Event,
    coroutines: Iterable[Awaitable[T]],
    queue: Queue[Awaitable[T]],
) -> None:
    for coroutine in coroutines:
        await queue.put(coroutine)
    await queue.join()
    end.set()


async def _as_completed(
    end: Event,
    output: Queue[T],
) -> list[T]:
    results = []
    while not (end.is_set() and output.empty()):
        try:
            results.append(await wait_for(output.get(), 0.5))
            output.task_done()
        except TimeoutError:
            pass
    return results


async def as_completed(
    coroutines: Union[Iterable[Awaitable[T]], Sequence[Awaitable[T]]],
    limit: int = max(12, DEFAULT_LIMIT),
) -> list[T]:
    queue: Queue[Awaitable[T]] = Queue(1)
    output: Queue[T] = Queue()
    end = Event()
    results = await gather(
        producer(end, coroutines, queue),
        *(worker(end, queue, output) for _ in range(limit)),
        _as_completed(end, output),
        end.wait(),
        return_exceptions=True,
    )
    raise_errors("Warning: as_completed() got multiple errors:", results)
    return next(filter(lambda result: isinstance(result, list), results))


def raise_errors(warning: str, results: Sequence[object]) -> None:
    if errors := list(
        filter(lambda result: isinstance(result, BaseException), results)
    ):
        error = errors.pop()
        if errors:
            print(
                warning,
                errors,
                file=stderr,
            )
        raise error  # type: ignore
