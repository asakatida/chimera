#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::float_arithmetic)]
#![allow(clippy::implicit_return)]
#![allow(clippy::integer_arithmetic)]
#![allow(clippy::missing_docs_in_private_items)]
#![allow(clippy::separated_literal_suffix)]

pub mod base;
pub mod complex;
pub mod imag;
pub mod natural;
pub mod negative;
pub mod number;
pub mod rational;
pub mod traits;
pub mod utils;

use core::fmt::Write;
use core::{ffi, fmt, mem, ptr};

use crate::negative::Negative;
use crate::number::Number;
use crate::traits::NumberBase;

#[inline]
#[must_use]
pub fn export_number(value: Number) -> *mut PythonNumber {
    let mut inner = mem::ManuallyDrop::new(value);
    let mut outer = mem::ManuallyDrop::new(PythonNumber {
        value: ptr::addr_of_mut!(*inner).cast(),
    });
    ptr::addr_of_mut!(*outer)
}

#[inline]
#[must_use]
pub fn fmt_code(_err: fmt::Error) -> i32 {
    1_i32
}

#[non_exhaustive]
pub struct Writer {
    pub buffer: *mut u8,
    pub len: isize,
}

#[allow(clippy::missing_trait_methods)]
impl fmt::Write for Writer {
    #[inline]
    fn write_str(&mut self, s: &str) -> fmt::Result {
        // SAFETY: not really safe
        unsafe {
            self.buffer
                .offset(self.len)
                .copy_from_nonoverlapping(s.as_bytes().as_ptr(), s.len());
        };
        self.len = self
            .len
            .checked_add(s.len().try_into().unwrap_or_default())
            .unwrap_or_default();
        Ok(())
    }
}

#[non_exhaustive]
#[repr(C)]
pub struct PythonNumber {
    pub value: *mut ffi::c_void,
}

impl PythonNumber {
    #[inline]
    #[must_use]
    #[allow(clippy::as_conversions)]
    fn get(&self) -> Number {
        // SAFETY:
        // - The pointer is valid for the lifetime of the PythonNumber.
        let inner = mem::ManuallyDrop::<Number>::new(unsafe { ptr::read(self.value.cast()) });
        mem::ManuallyDrop::into_inner(inner.clone())
    }
}

#[allow(clippy::empty_drop)]
impl Drop for PythonNumber {
    #[inline]
    fn drop(&mut self) {
        // // SAFETY:
        // // - The pointer is valid for the lifetime of the PythonNumber.
        // drop(unsafe { ptr::read(self.value.cast::<Number>()) });
    }
}

#[inline]
#[no_mangle]
pub extern "C" fn r_abs(left: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get().abs())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_add(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() + right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_not(left: &PythonNumber) -> *mut PythonNumber {
    export_number(!left.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_and(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() & right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_lshift(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() & right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_or(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() | right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_rshift(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() & right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_bit_xor(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() ^ right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_div(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() / right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_eq(left: &PythonNumber, right: &PythonNumber) -> bool {
    left.get() == right.get()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_gcd(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get().gcd(&right.get()))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_imag(left: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get().imag())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_is_complex(left: &PythonNumber) -> bool {
    left.get().is_complex()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_is_int(left: &PythonNumber) -> bool {
    left.get().is_int()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_is_nan(left: &PythonNumber) -> bool {
    left.get().is_nan()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_lt(left: &PythonNumber, right: &PythonNumber) -> bool {
    left.get() < right.get()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_modu(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() % right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_mul(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() * right.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_neg(left: &PythonNumber) -> *mut PythonNumber {
    export_number(-left.get())
}
#[inline]
#[no_mangle]
pub extern "C" fn r_pow(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get().pow(&right.get()))
}
#[inline]
#[no_mangle]
pub extern "C" fn r_repr(s: *mut u8, value: &PythonNumber) -> i32 {
    write!(Writer { buffer: s, len: 0 }, "{}", value.get())
        .err()
        .map(fmt_code)
        .unwrap_or_default()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_repr_len(value: &PythonNumber) -> usize {
    value.get().to_string().len()
}
#[inline]
#[no_mangle]
pub extern "C" fn r_sub(left: &PythonNumber, right: &PythonNumber) -> *mut PythonNumber {
    export_number(left.get() - right.get())
}

#[inline]
#[no_mangle]
#[allow(clippy::cast_possible_wrap)]
pub extern "C" fn r_cast_int(value: &PythonNumber) -> i64 {
    match value.get() {
        Number::Base(b) => b.value.try_into().unwrap_or(i64::MAX),
        Number::Natural(_) => i64::MAX,
        Number::Negative(Negative::Base(b)) => b
            .value
            .try_into()
            .ok()
            .and_then(i64::checked_neg)
            .unwrap_or(i64::MIN),
        Number::Rational(_)
        | Number::Negative(_)
        | Number::Imag(_)
        | Number::Complex(_)
        | Number::NaN => 0,
    }
}

#[inline]
#[no_mangle]
pub extern "C" fn r_cast_unsigned(value: &PythonNumber) -> u64 {
    match value.get() {
        Number::Base(b) => b.value,
        Number::Natural(_)
        | Number::Rational(_)
        | Number::Negative(_)
        | Number::Imag(_)
        | Number::Complex(_)
        | Number::NaN => 0,
    }
}

#[inline]
#[no_mangle]
pub extern "C" fn r_cast_float(value: &PythonNumber) -> f64 {
    match value.get() {
        Number::Base(b) => b.as_f64(),
        Number::Natural(b) => b.as_f64(),
        Number::Rational(b) => b.as_f64(),
        Number::Negative(Negative::Base(b)) => -b.as_f64(),
        Number::Negative(Negative::Natural(b)) => -b.as_f64(),
        Number::Negative(Negative::Rational(b)) => -b.as_f64(),
        Number::Imag(_) | Number::Complex(_) | Number::NaN => f64::NAN,
    }
}

#[inline]
#[no_mangle]
pub extern "C" fn r_create_number(value: u64) -> *mut PythonNumber {
    export_number(value.into())
}

/// # Safety
///
/// `number` is a valid pointer to a `PythonNumber`
#[inline]
#[no_mangle]
pub unsafe extern "C" fn r_copy_number(number: &PythonNumber) -> *mut PythonNumber {
    export_number(number.get())
}

/// # Safety
///
/// `number` is a valid pointer to a `PythonNumber`
/// release the memory allocated for the `PythonNumber`
#[inline]
#[no_mangle]
pub unsafe extern "C" fn r_delete_number(number: *mut PythonNumber) {
    drop(ptr::read(number));
}
