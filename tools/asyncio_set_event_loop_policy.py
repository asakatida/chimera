from asyncio import AbstractEventLoop, DefaultEventLoopPolicy
from asyncio import set_event_loop_policy as set_policy
from concurrent.futures import ThreadPoolExecutor


class EventLoopPolicy(DefaultEventLoopPolicy):
    def new_event_loop(self) -> AbstractEventLoop:
        loop = super().new_event_loop()
        loop.set_default_executor(ThreadPoolExecutor(max_workers=5))
        return loop


def set_event_loop_policy() -> None:
    set_policy(EventLoopPolicy())
