use core::{convert, fmt};

use crate::base::Base;
use crate::number::Number;

#[non_exhaustive]
pub enum MaybeBigUint {
    Base(Base),
    BigUint(num_bigint::BigUint),
}

/// # Errors
#[inline]
pub fn condense_bigint(i: &num_bigint::BigUint) -> Result<MaybeBigUint, convert::Infallible> {
    let mut value = i.to_u32_digits();
    while value.ends_with(&[0]) {
        value.pop();
    }
    if value.len() < 2 {
        TryInto::<u64>::try_into(value.pop().unwrap_or_default())
            .map(Into::into)
            .map(MaybeBigUint::Base)
    } else {
        Ok(MaybeBigUint::BigUint(num_bigint::BigUint::new(value)))
    }
}

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
