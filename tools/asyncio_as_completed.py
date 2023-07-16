from asyncio import FIRST_COMPLETED, Task, create_task, wait
from itertools import repeat
from os import cpu_count
from typing import Coroutine, Iterable, Iterator, TypeVar

T = TypeVar("T")

DEFAULT_LIMIT = max(cpu_count() or 0, 1) * 3


async def _list(iter: Iterator[Task[T]], cancel: bool) -> list[T]:
    try:
        return [await task for task in iter]
    finally:
        for task in iter:
            if cancel:
                try:
                    if not task.done():
                        task.cancel()
                except AttributeError:
                    pass
            try:
                await task
            except Exception:
                pass


async def _next(background_tasks: set[Task[T]], coroutine: Task[T]) -> T:
    try:
        done, pending = await wait(
            background_tasks | {coroutine}, return_when=FIRST_COMPLETED
        )
        return await done.pop()
    finally:
        background_tasks.clear()
        background_tasks |= done | pending


def _schedule_tasks(coroutines: Iterator[Task[T]], limit: int) -> Iterator[Task[T]]:
    background_tasks = {coroutine for coroutine, _ in zip(coroutines, range(limit - 1))}
    yield from map(_next, repeat(background_tasks), coroutines)
    yield from background_tasks


async def as_completed(
    coroutines: Iterable[Coroutine[object, object, T]],
    limit: int = DEFAULT_LIMIT,
    cancel: bool = False,
) -> list[T]:
    return await _list(_schedule_tasks(map(create_task, coroutines), limit), cancel)
