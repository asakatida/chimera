from asyncio import FIRST_COMPLETED, Event, Queue, Task
from asyncio import as_completed as _as_completed
from asyncio import create_task, wait
from collections.abc import AsyncIterator
from contextlib import AbstractAsyncContextManager
from os import cpu_count
from typing import Awaitable, Coroutine, Generator, Iterable, TypeVar, Union

T = TypeVar("T")
U = TypeVar("U")

DEFAULT_LIMIT = max(cpu_count() or 0, 4) // 4
TaskInput = Union[Generator[object, None, T], Coroutine[object, object, T]]


class TaskGroup(AbstractAsyncContextManager["TaskGroup[T]"]):
    def __init__(self) -> None:
        self.tasks: list[Task[object]] = []

    def __del__(self) -> None:
        for task in self.tasks:
            task.cancel()

    async def __aenter__(self) -> "TaskGroup[T]":
        return self

    async def __aexit__(self, *_: object) -> None:
        await self.cancel()

    async def cancel(self) -> None:
        for task in self.tasks:
            task.cancel()
        await self.wait()

    def create_task(self, coroutine: TaskInput[T]) -> Awaitable[T]:
        task = create_task(coroutine)
        self.tasks.append(task)
        return task

    async def wait(self) -> None:
        for task in _as_completed(self.tasks):
            await task


class _AsCompleted(AbstractAsyncContextManager[AsyncIterator[T]], AsyncIterator[T]):
    class QueueEmpty(Exception):
        pass

    def __init__(
        self, coroutines: Iterable[Awaitable[T]], limit: int = max(12, DEFAULT_LIMIT)
    ) -> None:
        self.queue: Queue[Awaitable[T]] = Queue(limit)
        self.output: Queue[T] = Queue(limit)
        self.end = Event()
        self.task_group: TaskGroup[object] = TaskGroup()
        self.background_tasks = [
            self.task_group.create_task(self.producer(coroutines)),
            *(self.task_group.create_task(self.worker()) for _ in range(limit)),
            self.task_group.create_task(self.end.wait()),
        ]

    async def __aenter__(self) -> AsyncIterator[T]:
        await self.task_group.__aenter__()
        return self

    async def __aexit__(self, *args: object) -> None:
        await self.task_group.__aexit__(*args)

    def __aiter__(self) -> AsyncIterator[T]:
        return self

    async def __anext__(self) -> T:
        while True:
            try:
                result = await self.queue_consume(self.output)
            except self.QueueEmpty:
                break
            self.output.task_done()
            return result
        if self.background_tasks:
            for background_result in _as_completed(self.background_tasks):
                await background_result
            self.background_tasks = []
        raise StopAsyncIteration

    async def queue_consume(self, queue: Queue[U]) -> U:
        done, pending = await wait(
            [queue.get(), self.end.wait()], return_when=FIRST_COMPLETED
        )
        set(map(lambda task: task.cancel(), pending))
        for task in done:
            if (result := task.result()) is not True:
                return result  # type: ignore
        raise self.QueueEmpty

    async def worker(self) -> None:
        while True:
            try:
                coroutine = await self.queue_consume(self.queue)
            except self.QueueEmpty:
                break
            await self.output.put(await coroutine)
            self.queue.task_done()

    async def producer(self, coroutines: Iterable[Awaitable[T]]) -> None:
        for coroutine in coroutines:
            await self.queue.put(coroutine)
        await self.queue.join()
        self.end.set()


def as_completed(
    coroutines: Iterable[Awaitable[T]], limit: int = max(12, DEFAULT_LIMIT)
) -> AbstractAsyncContextManager[AsyncIterator[T]]:
    return _AsCompleted(coroutines, limit)
