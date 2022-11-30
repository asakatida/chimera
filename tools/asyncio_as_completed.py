from asyncio import Queue, Task, create_task, wait_for
from os import cpu_count
from typing import Coroutine, Iterable, TypeVar

T = TypeVar("T")


async def worker(
    task: Task[None],
    queue: Queue[Coroutine[object, object, T]],
    output: Queue[T],
) -> None:
    while not (task.done() and queue.empty()):
        coroutine = await queue.get()
        await output.put(await coroutine)
        queue.task_done()


async def producer(
    coroutines: Iterable[Coroutine[object, object, T]],
    queue: Queue[Coroutine[object, object, T]],
) -> None:
    for coroutine in coroutines:
        await queue.put(coroutine)
    await queue.join()


async def _as_completed(
    task: Task[None],
    output: Queue[T],
) -> list[T]:
    results = []
    while not (task.done() and output.empty()):
        try:
            results.append(await wait_for(output.get(), 0.5))
            output.task_done()
        except TimeoutError:
            pass
    return results


async def as_completed(
    coroutines: Iterable[Coroutine[object, object, T]],
    limit: int = max(12, (cpu_count() or 1) // 4),
) -> list[T]:
    queue: Queue[Coroutine[object, object, T]] = Queue(1)
    output: Queue[T] = Queue()
    task = create_task(producer(coroutines, queue))
    results = await gather(
        task,
        *(worker(task, queue, output) for _ in range(limit)),
        _as_completed(task, output),
        return_exceptions=True,
    )
    if errors := list(
        filter(lambda result: isinstance(result, BaseException), results)
    ):
        raise errors[0]
    return next(filter(lambda result: isinstance(result, list), results))
