from asyncio import Queue, create_task, wait_for
from os import cpu_count
from typing import AsyncGenerator, Coroutine, Iterable, TypeVar

T = TypeVar("T")


async def worker(queue: Queue[Coroutine[object, object, T]], output: Queue[T]) -> None:
    while True:
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


async def as_completed(
    coroutines: Iterable[Coroutine[object, object, T]],
    limit: int = max(12, (cpu_count() or 1) // 4),
) -> AsyncGenerator[T, object]:
    queue: Queue[Coroutine[object, object, T]] = Queue(1)
    output: Queue[T] = Queue(1)
    workers = [create_task(worker(queue, output)) for _ in range(limit)]
    task = create_task(producer(coroutines, queue))
    while True:
        try:
            yield await wait_for(output.get(), 2)
            output.task_done()
        except TimeoutError:
            if task.done():
                break
    await wait_for(task, 2)
    [worker.cancel() for worker in workers]
