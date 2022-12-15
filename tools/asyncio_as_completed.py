from asyncio import Event, Queue, gather, wait_for
from os import cpu_count
from sys import stderr
from typing import Coroutine, Iterable, TypeVar

T = TypeVar("T")

DEFAULT_LIMIT = max(cpu_count() or 0, 4) // 4


async def worker(
    end: Event,
    queue: Queue[Coroutine[object, object, T]],
    output: Queue[T],
) -> None:
    while not (end.is_set() and queue.empty()):
        coroutine = await queue.get()
        await output.put(await coroutine)
        queue.task_done()


async def producer(
    end: Event,
    coroutines: Iterable[Coroutine[object, object, T]],
    queue: Queue[Coroutine[object, object, T]],
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
    coroutines: Iterable[Coroutine[object, object, T]],
    limit: int = max(12, DEFAULT_LIMIT),
) -> list[T]:
    if limit != DEFAULT_LIMIT:
        print(
            "Warning: as_completed() is using custom tuning",
            limit,
            "instead of default",
            DEFAULT_LIMIT,
            file=stderr,
        )
    queue: Queue[Coroutine[object, object, T]] = Queue(1)
    output: Queue[T] = Queue()
    end = Event()
    results = await gather(
        producer(end, coroutines, queue),
        *(worker(end, queue, output) for _ in range(limit)),
        _as_completed(end, output),
        end.wait(),
        return_exceptions=True,
    )
    if errors := list(
        filter(lambda result: isinstance(result, BaseException), results)
    ):
        raise errors[0]
    return next(filter(lambda result: isinstance(result, list), results))
