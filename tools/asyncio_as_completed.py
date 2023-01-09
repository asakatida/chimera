from asyncio import FIRST_COMPLETED, Event, Queue
from asyncio import as_completed as _as_completed
from asyncio import create_task, wait
from collections.abc import AsyncIterable, AsyncIterator
from os import cpu_count
from typing import Awaitable, Iterable, TypeVar

T = TypeVar("T")
U = TypeVar("U")

DEFAULT_LIMIT = max(cpu_count() or 0, 4) // 4


class _AsCompleted:
    class QueueEmpty(Exception):
        pass

    def __init__(
        self, coroutines: Iterable[Awaitable[T]], limit: int = max(12, DEFAULT_LIMIT)
    ) -> None:
        self.queue: Queue[Awaitable[T]] = Queue(limit)
        self.output: Queue[T] = Queue(limit)
        self.end = Event()
        self.background_tasks = [
            create_task(self.producer(coroutines)),
            *(create_task(self.worker()) for _ in range(limit)),
            create_task(self.end.wait()),
        ]

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
) -> AsyncIterable[T]:
    return _AsCompleted(coroutines, limit)
