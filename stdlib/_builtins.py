"""builtins.py."""


def __build_class__(
        func: object,
        name: str,
        *bases: type,
        metaclass: type=None,
        **kwds: object) -> type:
    locals = dict()
    func(locals)
    return type(name, bases, locals)


__loader__ = None
__name__ = "builtins"
__package__ = ""
__spec__ = None


class BaseException(object):
    """BaseException."""


class SystemExit(BaseException):
    """SystemExit."""


class KeyboardInterrupt(BaseException):
    """KeyboardInterrupt."""


class GeneratorExit(BaseException):
    """GeneratorExit."""


class Exception(BaseException):
    """Exception."""


class StopIteration(Exception):
    """StopIteration."""


class StopAsyncIteration(Exception):
    """StopAsyncIteration."""


class ArithmeticError(Exception):
    """ArithmeticError."""


class FloatingPointError(ArithmeticError):
    """FloatingPointError."""


class OverflowError(ArithmeticError):
    """OverflowError."""


class ZeroDivisionError(ArithmeticError):
    """ZeroDivisionError."""


class AssertionError(Exception):
    """AssertionError."""


class AttributeError(Exception):
    """AttributeError."""


class BufferError(Exception):
    """BufferError."""


class EOFError(Exception):
    """EOFError."""


class ModuleNotFoundError(ImportError):
    """ModuleNotFoundError."""


class LookupError(Exception):
    """LookupError."""


class IndexError(LookupError):
    """IndexError."""


class KeyError(LookupError):
    """KeyError."""


class MemoryError(Exception):
    """MemoryError."""


class NameError(Exception):
    """NameError."""


class UnboundLocalError(NameError):
    """UnboundLocalError."""


class OSError(Exception):
    """OSError."""


class BlockingIOError(OSError):
    """BlockingIOError."""


class ChildProcessError(OSError):
    """ChildProcessError."""


class ConnectionError(OSError):
    """ConnectionError."""


class BrokenPipeError(ConnectionError):
    """BrokenPipeError."""


class ConnectionAbortedError(ConnectionError):
    """ConnectionAbortedError."""


class ConnectionRefusedError(ConnectionError):
    """ConnectionRefusedError."""


class ConnectionResetError(ConnectionError):
    """ConnectionResetError."""


class FileExistsError(OSError):
    """FileExistsError."""


class FileNotFoundError(OSError):
    """FileNotFoundError."""


class InterruptedError(OSError):
    """InterruptedError."""


class IsADirectoryError(OSError):
    """IsADirectoryError."""


class NotADirectoryError(OSError):
    """NotADirectoryError."""


class PermissionError(OSError):
    """PermissionError."""


class ProcessLookupError(OSError):
    """ProcessLookupError."""


class TimeoutError(OSError):
    """TimeoutError."""


class ReferenceError(Exception):
    """ReferenceError."""


class RuntimeError(Exception):
    """RuntimeError."""


class NotImplementedError(RuntimeError):
    """NotImplementedError."""


class RecursionError(RuntimeError):
    """RecursionError."""


class SyntaxError(Exception):
    """SyntaxError."""


class IndentationError(SyntaxError):
    """IndentationError."""


class TabError(IndentationError):
    """TabError."""


class SystemError(Exception):
    """SystemError."""


class ValueError(Exception):
    """ValueError."""


class UnicodeError(ValueError):
    """UnicodeError."""


class UnicodeDecodeError(UnicodeError):
    """UnicodeDecodeError."""


class UnicodeEncodeError(UnicodeError):
    """UnicodeEncodeError."""


class UnicodeTranslateError(UnicodeError):
    """UnicodeTranslateError."""


class Warning(Exception):
    """Warning."""


class DeprecationWarning(Warning):
    """DeprecationWarning."""


class PendingDeprecationWarning(Warning):
    """PendingDeprecationWarning."""


class RuntimeWarning(Warning):
    """RuntimeWarning."""


class SyntaxWarning(Warning):
    """SyntaxWarning."""


class UserWarning(Warning):
    """UserWarning."""


class FutureWarning(Warning):
    """FutureWarning."""


class ImportWarning(Warning):
    """ImportWarning."""


class UnicodeWarning(Warning):
    """UnicodeWarning."""


class BytesWarning(Warning):
    """BytesWarning."""


class ResourceWarning(Warning):
    """ResourceWarning."""


Ellipsis = ...


class NotImplemented(object):
    """NotImplemented."""


def abs(x: object) -> object:
    """abs."""
    return x.__abs__()


def all(iterable: object) -> bool:
    """all."""
    for element in iterable:
        if not element:
            return False
    return True


def any(iterable: object) -> bool:
    """any."""
    for element in iterable:
        if element:
            return True
    return False


def ascii(object: object) -> str:
    """ascii."""
    return repr(object)


def bin(x: object) -> str:
    """bin."""
    return repr(int(x))


class bytearray(object):
    """bytearray."""

    def __init__(source=None, encoding=None, errors=None) -> None:
        """__init__."""


def callable(object: object) -> bool:
    """callable."""
    return hasattr(object, '__call__')


def chr(i: int) -> str:
    """chr."""
    return '\0'


def classmethod(method: object) -> object:
    """classmethod."""
    def f(self: object, *args: object, **kwargs: object) -> object:
        return method(type(self), *args, **kwargs)
    return f


class complex(object):
    """complex."""


def delattr(object: object, name: str) -> None:
    """delattr."""
    object.__delattr__(name)


class dict(object):
    """dict."""

    def __init__(self, *args, **kwargs) -> None:
        """__init__."""
        if args:
            self.init_args(*args)
        else:
            self.init_kwargs(**kwargs)

    def __contains__(self, key) -> bool:
        """__contains__."""
        return any(
            k == key for k, _ in self.buckets[hash(key) % len(self.buckets)])

    def __delitem__(self, key) -> None:
        """__delitem__."""
        self.buckets[hash(key) % len(self.buckets)] = filter(
            lambda pair: pair[0] != key,
            self.buckets[hash(key) % len(self.buckets)])

    def __eq__(self, value) -> bool:
        """__eq__."""
        return len(self) == len(value) and all(
            key in value and self[key] == value[key] for key in self)

    def __ge__(self, value) -> bool:
        """__ge__."""
        return False

    def __getitem__(self, key):
        """__getitem__."""
        return next(filter(
            lambda pair: pair[0] == key,
            self.buckets[hash(key) % len(self.buckets)]))[1]

    def __gt__(self, value) -> bool:
        """__gt__."""
        return False

    def __iter__(self):
        """__iter__."""
        return iter(self.keys())

    def __le__(self, value) -> bool:
        """__le__."""
        return False

    def __len__(self):
        """__len__."""
        return sum(map(len, self.buckets))

    def __lt__(self, value) -> bool:
        """__lt__."""
        return False

    def __ne__(self, value) -> bool:
        """__ne__."""
        return len(self) != len(value) or any(
            key not in value or self[key] != value[key] for key in self)

    def __repr__(self) -> str:
        """__repr__."""
        return (
            '{' +
            ', '.join((f'{key}: {value}' for key, value in self.values())) +
            '}')

    def __setitem__(self, key, value) -> None:
        """__setitem__."""
        self.buckets[hash(key) % len(self.buckets)].append((key, value))

    def clear(self) -> None:
        """clear."""
        self.buckets = ()

    def copy(self) -> dict:
        """copy."""
        return dict(self.items())

    def fromkeys(iterable, value=None):
        """fromkeys."""
        return dict((key, value) for key in iterable)

    def get(self, key, default=None):
        """get."""
        try:
            return self[key]
        except KeyError:
            return default

    def items(self):
        """
        items.

        D.items() -> a set-like object providing a view on D's items.
        """

    def keys(self):
        """
        keys.

        D.keys() -> a set-like object providing a view on D's keys.
        """

    def pop(self, key, default=None):
        """
        pop.

        D.pop(k[,d]) -> v, remove specified key and return the corresponding
        value.  If key is not found, d is returned if given, otherwise KeyError
        is raised.
        """

    def popitem(self):
        """popitem."""
        if not self:
            raise KeyError
        key = next(self)
        return (key, self.pop(key))

    def setdefault(self, key, default=None):
        """setdefault."""
        if key not in self:
            self[key] = default
        return self.get(key, default)

    def update(self, iterable=None, **kwargs):
        """update."""
        if iterable is not None:
            if hasattr(iterable, "keys"):
                for key in iterable.keys():
                    self[key] = iterable[key]
            else:
                for key, value in iterable:
                    self[key] = value
        for key in kwargs:
            self[key] = kwargs[key]

    def values(self):
        """
        values.

        D.values() -> an object providing a view on D's values
        """

    __hash__ = None


def dir(object: object=None) -> object:
    """dir."""
    return object.__dir__()


def divmod(a: object, b: object) -> object:
    """divmod."""
    return a // b, a % b


def enumerate(sequence: object, start: object=0) -> object:
    """enumerate."""
    n = start
    for elem in sequence:
        yield n, elem
        n += 1


def filter(function, iterable) -> object:
    """filter."""
    if function is None:
        return (item for item in iterable if item)
    return (item for item in iterable if function(item))


format = None


class frozenset(object):
    """frozenset."""


def globals() -> "dict":
    """globals."""
    return dict()


def hasattr(object: object, attr: str) -> bool:
    """hasattr."""
    return attr in object.__dict__.keys()


def hash(object: object) -> int:
    """hash."""
    return object.__hash__()


help = None
hex = None
input = None
isinstance = None
issubclass = None


def iter(object: object) -> object:
    """iter."""
    return object.__iter__()


def len(object: object) -> int:
    """len."""
    return object.__len__()


list = None
locals = globals
map = None
max = None
memoryview = None
min = None


def next(object: object) -> object:
    """next."""
    return object.__next__()


oct = None
open = None


def ord(c: str) -> int:
    """ord."""
    return 0


pow = None


class property(object):
    """property."""

    def __init__(self, fget=None, fset=None, fdel=None, doc: str=None) -> None:
        """__init__."""
        self._fget = fget
        self._fset = fset
        self._fdel = fdel
        self.__doc__ = doc or fget.__doc__

    def getter(self, fget) -> None:
        """getter."""
        self._fget = fget

    def setter(self, fset) -> None:
        """setter."""
        self._fset = fset

    def deleter(self, fdel) -> None:
        """deleter."""
        self._fdel = fdel


class range(object):
    """range."""

    def __init__(self, start: int, stop: int=None, step: int=None) -> None:
        """__init__."""
        if stop is None:
            self._start = 0
            self._stop = start
            self._step = 1
        else:
            self._start = start or 0
            self._stop = stop
            self._step = step or 1

    def __len__(self) -> int:
        """__len__."""
        return (self._stop - self._start) // self._step

    @property
    def start(self) -> int:
        """start."""
        return self._start

    @property
    def stop(self) -> int:
        """stop."""
        return self._stop

    @property
    def step(self) -> int:
        """step."""
        return self._step


def repr(object: object) -> str:
    """repr."""
    return object.__repr__()


reversed = None
round = None


class set(object):
    """set."""


def setattr(object: object, name: str, value) -> None:
    """setattr."""
    object.__setattr__(name, value)


class slice(object):
    """slice."""

    def __init__(self, start: int, stop: int=None, step: int=None) -> None:
        """__init__."""
        if stop is None:
            self._start = None
            self._stop = start
            self._step = None
        else:
            self._start = start
            self._stop = stop
            self._step = step

    @property
    def start(self) -> int:
        """start."""
        return self._start

    @property
    def stop(self) -> int:
        """stop."""
        return self._stop

    @property
    def step(self) -> int:
        """step."""
        return self._step


def sorted(iterable, *, key=None, reverse=False) -> list:
    """sorted."""
    return list()


staticmethod = None


def sum(iterable, start=0) -> object:
    """sum."""
    for elem in iterable:
        start += elem
    return start


def vars(object):
    """vars."""
    return object.__dict__


def zip(*iterables):
    """zip."""
    if not iterables:
        return
    iterators = [iter(it) for it in iterables]
    while True:
        try:
            yield tuple(map(next, iterators))
        except StopIteration:
            return
