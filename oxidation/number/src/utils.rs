use core::fmt;

use crate::number::Number;

/// # Errors
#[allow(clippy::as_conversions)]
#[inline]
pub fn fmt_ptr<T>(t: &T, f: &mut fmt::Formatter) -> fmt::Result {
    fmt::Pointer::fmt(&(t as *const _), f)
}

#[inline]
#[must_use]
pub fn gcd<T: Into<Number>>(left: T, right: T) -> Number {
    let mut a_prime: Number = left.into();
    let mut b_prime: Number = right.into();
    while b_prime > 0 {
        let temp = b_prime.clone();
        b_prime = a_prime % b_prime;
        a_prime = temp;
    }
    a_prime
}

#[inline]
#[must_use]
pub fn rem<T: Into<Number>>(left: T, right: T) -> Number {
    let mut remainder: Number = left.into();
    let divisor: Number = right.into();
    while divisor.clone() < remainder {
        remainder = remainder - divisor.clone();
    }
    remainder
}
