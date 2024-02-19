#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::implicit_return)]
#![allow(clippy::missing_docs_in_private_items)]

use core::fmt::{Formatter, Pointer, Result};
use core::ptr::from_ref;

use crate::number::Number;

/// # Errors
#[allow(clippy::as_conversions)]
#[inline]
pub fn fmt_ptr<T>(pointer: &T, formatter: &mut Formatter) -> Result {
    Pointer::fmt(&from_ref(pointer), formatter)
}

#[inline]
#[must_use]
pub fn gcd<T: Into<Number>, U: Into<Number>>(left: T, right: U) -> Number {
    let mut a_prime: Number = left.into();
    let mut b_prime: Number = right.into();
    while b_prime > 0 {
        let temp = b_prime.clone();
        b_prime = a_prime % b_prime;
        a_prime = temp;
    }
    a_prime
}
