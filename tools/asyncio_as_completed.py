from asyncio import Task, get_event_loop
from os import cpu_count
from typing import AsyncGenerator, Coroutine, Iterable, TypeVar

T = TypeVar("T")


async def as_completed_wait(task: Task[T]) -> T:
    await task
    if error := task.exception():
        raise error
    return task.result()


async def as_completed_each(
    errors: list[Exception], task: Task[T]
) -> AsyncGenerator[T, object]:
    try:
        yield await as_completed_wait(task)
    except Exception as error:
        errors.append(error)


async def as_completed(
    routines: Iterable[Coroutine[object, object, T]],
    limit: int = max(4, (cpu_count() or 1) // 4),
) -> AsyncGenerator[T, object]:
    errors: list[Exception] = []
    running: list[Task[T]] = []
    for routine in routines:
        running.append(get_event_loop().create_task(routine))
        if len(running) >= limit:
            async for elem in as_completed_each(errors, running.pop(0)):
                yield elem
            for task in running:
                if task.done():
                    async for elem in as_completed_each(errors, task):
                        yield elem
                    running.remove(task)
    for task in running:
        async for elem in as_completed_each(errors, task):
            yield elem
    if errors:
        raise errors[0]
