from asyncio import FIRST_COMPLETED, Event, Queue, Task
from asyncio import as_completed as _as_completed
from asyncio import create_task, wait
from collections.abc import AsyncIterator
from os import cpu_count
from typing import Awaitable, Coroutine, Iterable, Optional, TypeVar

T = TypeVar("T")
U = TypeVar("U")

DEFAULT_LIMIT = max(cpu_count() or 0, 4) // 4


def cancel_tasks(tasks: Iterable[Task[object]]) -> None:
    set(map(lambda task: task.cancel(), filter(lambda task: not task.done(), tasks)))


class TaskGroupInternal:
    def __init__(self, task_group: "TaskGroup") -> None:
        self.task_group = task_group

    def create_task(self, coroutine: Coroutine[object, object, T]) -> Task[T]:
        task: Task[T] = create_task(coroutine)
        self.task_group.tasks.append(task)
        return task


class TaskGroup:
    def __init__(self) -> None:
        self.tasks: list[Task[object]] = []

    def __enter__(self) -> TaskGroupInternal:
        return TaskGroupInternal(self)

    def __exit__(self, *_: object) -> None:
        cancel_tasks(self.tasks)


class _AsCompleted(AsyncIterator[T]):
    class QueueEmpty(Exception):
        pass

    def __init__(
        self, coroutines: Iterable[Awaitable[T]], limit: int = max(12, DEFAULT_LIMIT)
    ) -> None:
        self.queue: Queue[Awaitable[T]] = Queue(limit)
        self.output: Queue[T] = Queue(limit)
        self.end = Event()
        self._background_task: Optional[Task[None]] = create_task(
            self._background(coroutines, limit)
        )

    def __del__(self) -> None:
        if self._background_task is not None:
            self._background_task.cancel()

    def __aiter__(self) -> AsyncIterator[T]:
        return self

    async def __anext__(self) -> T:
        while True:
            try:
                result = await self._queue_consume(self.output)
            except self.QueueEmpty:
                break
            self.output.task_done()
            return result
        if self._background_task is not None:
            await self._background_task
            self._background_task = None
        raise StopAsyncIteration

    async def _background(self, coroutines: Iterable[Awaitable[T]], limit: int) -> None:
        with TaskGroup() as task_group:
            background_tasks = [
                task_group.create_task(self._worker()) for _ in range(limit)
            ]
            await self._producer(coroutines)
            for task in _as_completed(background_tasks):
                await task

    async def _producer(self, coroutines: Iterable[Awaitable[T]]) -> None:
        try:
            for coroutine in coroutines:
                await self.queue.put(coroutine)
            await self.queue.join()
        finally:
            self.end.set()

    async def _queue_consume(self, queue: Queue[U]) -> U:
        done, pending = await wait(
            [queue.get(), self.end.wait()], return_when=FIRST_COMPLETED
        )
        cancel_tasks(pending)
        for task in done:
            if (result := task.result()) is not True:
                return result  # type: ignore
        raise self.QueueEmpty

    async def _worker(self) -> None:
        while True:
            try:
                coroutine = await self._queue_consume(self.queue)
            except self.QueueEmpty:
                break
            await self.output.put(await coroutine)
            self.queue.task_done()


def as_completed(
    coroutines: Iterable[Awaitable[T]], limit: int = max(12, DEFAULT_LIMIT)
) -> AsyncIterator[T]:
    return _AsCompleted(coroutines, limit)
