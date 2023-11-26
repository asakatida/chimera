#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::exhaustive_enums)]
#![allow(clippy::float_arithmetic)]
#![allow(clippy::implicit_return)]
#![allow(clippy::missing_docs_in_private_items)]
#![allow(clippy::mutex_atomic)]
#![allow(clippy::separated_literal_suffix)]
#![allow(clippy::single_call_fn)]

pub mod base;
pub mod complex;
pub mod imag;
pub mod natural;
pub mod negative;
pub mod number;
pub mod rational;
pub mod traits;
pub mod utils;

use core::fmt;
use core::fmt::Write;
use core::hash::BuildHasher;
use num_traits::{Pow, ToPrimitive};
use std::sync::OnceLock;
use std::{collections, sync, thread};

use crate::number::Number;
use crate::traits::NumberBase;

#[derive(Debug)]
enum Event {
    End,
    Del(u64),
    Get(u64, sync::Arc<(sync::Condvar, sync::Mutex<(bool, Number)>)>),
    New(Number, sync::Arc<(sync::Condvar, sync::Mutex<(bool, u64)>)>),
    Syn(sync::Arc<(sync::Condvar, sync::Mutex<bool>)>),
}

#[allow(clippy::expect_used)]
fn gc_loop(receiver: &sync::mpsc::Receiver<Event>) {
    let mut heap = collections::HashSet::<Number>::new();
    let mut refs = collections::BTreeMap::<u64, usize>::new();
    while let Ok(event) = receiver.recv() {
        match event {
            Event::Del(key) => {
                refs.entry(key).and_modify(|reference| *reference -= 1);
                if Some(0) == refs.get(&key).copied() {
                    heap.remove(
                        &heap
                            .iter()
                            .find(|num| heap.hasher().hash_one(num) == key)
                            .cloned()
                            .expect("Event::Del(key)"),
                    );
                    refs.remove(&key);
                }
            }
            Event::Get(key, wait) => {
                if let Ok(mut started) = wait.1.lock() {
                    *started = (
                        true,
                        heap.iter()
                            .find(|num| heap.hasher().hash_one(num) == key)
                            .cloned()
                            .expect("Event::Get(key, wait)"),
                    );
                    wait.0.notify_one();
                } else {
                    return;
                }
            }
            Event::New(value, wait) => {
                if let Ok(mut started) = wait.1.lock() {
                    let key = heap.hasher().hash_one(value.clone());
                    if heap.insert(value) {
                        refs.insert(key, 1);
                    } else {
                        refs.entry(key).and_modify(|reference| *reference += 1);
                    };
                    *started = (true, key);
                    wait.0.notify_one();
                } else {
                    return;
                }
            }
            Event::Syn(wait) => {
                if let Ok(mut started) = wait.1.lock() {
                    *started = true;
                    wait.0.notify_one();
                } else {
                    return;
                }
            }
            Event::End => {
                return;
            }
        }
    }
}

static EVENTS: OnceLock<sync::mpsc::SyncSender<Event>> = OnceLock::new();

fn events() -> &'static sync::mpsc::SyncSender<Event> {
    EVENTS.get_or_init(|| {
        let (sender, receiver) = sync::mpsc::sync_channel(12);
        thread::spawn(move || gc_loop(&receiver));
        sender
    })
}

/// # Panics
#[allow(clippy::unwrap_used)]
#[inline]
#[must_use]
fn export_number(value: Number) -> u64 {
    let wait = sync::Arc::new((sync::Condvar::new(), sync::Mutex::new((false, 0_u64))));
    events()
        .send(Event::New(value, sync::Arc::clone(&wait)))
        .unwrap();
    let mut started = wait.1.lock().unwrap();
    while !started.0 {
        started = wait.0.wait(started).unwrap();
    }
    started.1
}

#[inline]
#[must_use]
fn fmt_code(_err: fmt::Error) -> i32 {
    1_i32
}

/// # Panics
#[allow(clippy::unwrap_used)]
#[inline]
#[must_use]
fn get(key: u64) -> Number {
    let wait = sync::Arc::new((
        sync::Condvar::new(),
        sync::Mutex::new((false, Number::new(0))),
    ));
    events()
        .send(Event::Get(key, sync::Arc::clone(&wait)))
        .unwrap();
    let mut started = wait.1.lock().unwrap();
    while !started.0 {
        started = wait.0.wait(started).unwrap();
    }
    started.1.clone()
}

struct Writer {
    buffer: *mut u8,
    len: usize,
    capacity: usize,
}

#[allow(clippy::missing_trait_methods)]
#[allow(clippy::question_mark_used)]
impl fmt::Write for Writer {
    #[inline]
    fn write_str(&mut self, string: &str) -> fmt::Result {
        if string.is_empty() {
            return Ok(());
        }
        let bytes = string.as_bytes();
        let length = bytes.len();
        if self.capacity.checked_sub(self.len).ok_or(fmt::Error)? < length {
            return Err(fmt::Error);
        }
        let buffer = self.len.try_into().ok().ok_or(fmt::Error).map(|len| {
            // SAFETY: depends on capacity being honest
            unsafe { self.buffer.offset(len) }
        })?;
        // SAFETY: depends on capacity being honest
        unsafe {
            buffer.copy_from_nonoverlapping(bytes.as_ptr(), length);
        };
        self.len = self.len.checked_add(length).ok_or(fmt::Error)?;
        Ok(())
    }
}

#[inline]
#[no_mangle]
pub extern "C" fn r_abs(left: u64) -> u64 {
    export_number(get(left).abs())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_add(left: u64, right: u64) -> u64 {
    export_number(get(left) + get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_not(left: u64) -> u64 {
    export_number(!get(left))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_and(left: u64, right: u64) -> u64 {
    export_number(get(left) & get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_lshift(left: u64, right: u64) -> u64 {
    export_number(get(left) & get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_or(left: u64, right: u64) -> u64 {
    export_number(get(left) | get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_rshift(left: u64, right: u64) -> u64 {
    export_number(get(left) & get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_xor(left: u64, right: u64) -> u64 {
    export_number(get(left) ^ get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_div(left: u64, right: u64) -> u64 {
    export_number(get(left) / get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_eq(left: u64, right: u64) -> bool {
    get(left) == get(right)
}
#[inline]
#[no_mangle]
pub extern "C" fn r_div_floor(left: u64, right: u64) -> u64 {
    export_number(get(left).div_floor(get(right)))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_floor_div(left: u64, right: u64) -> u64 {
    export_number(get(left).div_floor(get(right)))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_gcd(left: u64, right: u64) -> u64 {
    export_number(get(left).gcd(get(right)))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_imag(left: u64) -> u64 {
    export_number(get(left).imag())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_is_complex(left: u64) -> bool {
    get(left).is_complex()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_is_int(left: u64) -> bool {
    get(left).is_int()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_is_nan(left: u64) -> bool {
    get(left).is_nan()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_lt(left: u64, right: u64) -> bool {
    get(left) < get(right)
}
#[inline]
#[no_mangle]
pub extern "C" fn r_modu(left: u64, right: u64) -> u64 {
    export_number(get(left) % get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_mul(left: u64, right: u64) -> u64 {
    export_number(get(left) * get(right))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_neg(left: u64) -> u64 {
    export_number(-get(left))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_pow(left: u64, right: u64) -> u64 {
    export_number(get(left).pow(get(right)))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_mod_pow(base: u64, exp: u64, modu: u64) -> u64 {
    export_number(get(base).mod_pow(get(exp), get(modu)))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_repr(buffer: *mut u8, capacity: usize, value: u64) -> i32 {
    write!(
        Writer {
            buffer,
            len: 0,
            capacity,
        },
        "{}",
        get(value)
    )
    .err()
    .map(fmt_code)
    .unwrap_or_default()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_repr_len(value: u64) -> usize {
    get(value).to_string().len()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_sub(left: u64, right: u64) -> u64 {
    export_number(get(left) - get(right))
}

#[inline]
#[no_mangle]
#[allow(clippy::cast_possible_wrap)]
pub extern "C" fn r_cast_int(value: u64) -> i64 {
    get(value).to_i64().unwrap_or(0)
}

#[inline]
#[no_mangle]
pub extern "C" fn r_cast_unsigned(value: u64) -> u64 {
    get(value).to_u64().unwrap_or(0)
}

#[inline]
#[no_mangle]
pub extern "C" fn r_cast_float(value: u64) -> f64 {
    get(value).to_f64().unwrap_or(f64::NAN)
}

#[inline]
#[no_mangle]
pub extern "C" fn r_create_number(value: u64) -> u64 {
    export_number(value.into())
}

#[inline]
#[no_mangle]
pub extern "C" fn r_copy_number(number: u64) -> u64 {
    export_number(get(number))
}

/// # Panics
#[allow(clippy::unwrap_used)]
#[inline]
#[no_mangle]
pub extern "C" fn r_delete_number(number: u64) {
    events().send(Event::Del(number)).unwrap();
}

/// # Panics
#[allow(clippy::unwrap_used)]
#[inline]
#[no_mangle]
pub extern "C" fn r_vm_clear() {
    let wait = sync::Arc::new((sync::Condvar::new(), sync::Mutex::new(false)));
    events().send(Event::Syn(sync::Arc::clone(&wait))).unwrap();
    let mut started = wait.1.lock().unwrap();
    while !*started {
        started = wait.0.wait(started).unwrap();
    }
}

/// # Panics
#[allow(clippy::unwrap_used)]
#[inline]
#[no_mangle]
pub extern "C" fn r_vm_end() {
    events().send(Event::End).unwrap();
}
