from asyncio import Task, get_event_loop
from os import cpu_count
from typing import AsyncGenerator, Coroutine, Iterable, TypeVar

T = TypeVar("T")


async def as_completed_each(
    errors: list[BaseException], task: Task[T]
) -> AsyncGenerator[T, object]:
    await task
    if error := task.exception():
        errors.append(error)
    else:
        yield task.result()


async def as_completed(
    routines: Iterable[Coroutine[object, object, T]],
    limit: int = max(12, (cpu_count() or 1) // 4),
) -> AsyncGenerator[T, object]:
    errors: list[BaseException] = []
    running: list[Task[T]] = []
    for _ in map(running.append, map(get_event_loop().create_task, routines)):  # type: ignore
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
