from asyncio import FIRST_COMPLETED, Task
from asyncio import as_completed as _as_completed
from asyncio import create_task, gather, wait
from itertools import repeat
from os import cpu_count
from typing import Awaitable, Coroutine, Iterable, Iterator, TypeVar

T = TypeVar("T")

DEFAULT_LIMIT = max(cpu_count() or 0, 4) // 4


class TaskGroupInternal:
    def __init__(self) -> None:
        self.tasks: set[Task[object]] = set()

    def __del__(self) -> None:
        set(map(lambda task: task.cancel(), self.tasks))

    def create_task(self, coroutine: Coroutine[object, object, T]) -> Task[T]:
        task: Task[T] = create_task(coroutine)
        self.tasks.add(task)
        return task


async def _next(background_tasks: set[Task[T]], coroutine: Task[T]) -> Task[T]:
    done = pending = background_tasks.copy()
    try:
        done, pending = await wait(background_tasks, return_when=FIRST_COMPLETED)
        return done.pop()
    finally:
        background_tasks.clear()
        background_tasks |= done | pending | {coroutine}


def _schedule_tasks(
    coroutines: Iterator[Task[T]], limit: int
) -> Iterable[Awaitable[T]]:
    background_tasks = {coroutine for coroutine, _ in zip(coroutines, range(limit - 1))}
    yield from map(_unwrap, map(_next, repeat(background_tasks), coroutines))
    yield from _as_completed(background_tasks)


async def _unwrap(task: Awaitable[Task[T]]) -> T:
    return await (await task)


async def a_list(iter: Iterable[Awaitable[T]]) -> list[T]:
    try:
        return [await task for task in iter]
    finally:
        tasks = map(create_task, iter)  # type: ignore
        set(map(lambda task: task.cancel(), tasks))
        await gather(*tasks, return_exceptions=True)


def as_completed(
    coroutines: Iterable[Coroutine[object, object, T]],
    limit: int = max(12, DEFAULT_LIMIT),
) -> Iterable[Awaitable[T]]:
    task_group = TaskGroupInternal()
    yield from _schedule_tasks(
        map(lambda task: task_group.create_task(task), coroutines), limit
    )
