#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::implicit_return)]
#![allow(clippy::integer_arithmetic)]
#![allow(clippy::missing_docs_in_private_items)]

use core::{cmp, fmt, ops};

use crate::natural::Natural;
use crate::negative::Negative;
use crate::number::Number;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::fmt_ptr;

#[derive(Clone, Copy, Debug, Default, Eq, Ord, PartialEq, PartialOrd)]
#[non_exhaustive]
pub struct Base {
    value: u64,
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Base {
    #[inline]
    fn eq(&self, other: &u64) -> bool {
        self.value == *other
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialOrd<u64> for Base {
    #[inline]
    fn partial_cmp(&self, other: &u64) -> Option<cmp::Ordering> {
        self.value.partial_cmp(other)
    }
}

impl Base {
    #[inline]
    #[must_use]
    pub fn new(i: u64) -> Self {
        Self { value: i }
    }
}

#[allow(clippy::missing_trait_methods)]
impl num_traits::ToPrimitive for Base {
    #[inline]
    fn to_i64(&self) -> Option<i64> {
        self.value.to_i64()
    }
    #[inline]
    fn to_u64(&self) -> Option<u64> {
        self.value.to_u64()
    }
    #[inline]
    fn to_f64(&self) -> Option<f64> {
        self.value.to_f64()
    }
}

impl fmt::Binary for Base {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Binary::fmt(&self.value, f)
    }
}

impl fmt::Display for Base {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Display::fmt(&self.value, f)
    }
}

impl fmt::LowerExp for Base {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::LowerExp::fmt(&self.value, f)
    }
}

impl fmt::LowerHex for Base {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::LowerHex::fmt(&self.value, f)
    }
}

impl fmt::Octal for Base {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Octal::fmt(&self.value, f)
    }
}

impl fmt::Pointer for Base {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt_ptr(self, f)
    }
}

impl fmt::UpperExp for Base {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::UpperExp::fmt(&self.value, f)
    }
}

impl fmt::UpperHex for Base {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::UpperHex::fmt(&self.value, f)
    }
}

impl ops::Add for Base {
    type Output = Number;
    #[inline]
    fn add(self, other: Self) -> Self::Output {
        match self.value.overflowing_add(other.value) {
            (i, false) => i.into(),
            (_, true) => Natural::from(self) + other.into(),
        }
    }
}

impl ops::BitAnd for Base {
    type Output = Number;
    #[inline]
    fn bitand(self, other: Self) -> Self::Output {
        (self.value & other.value).into()
    }
}

impl ops::BitOr for Base {
    type Output = Number;
    #[inline]
    fn bitor(self, other: Self) -> Self::Output {
        (self.value | other.value).into()
    }
}

impl ops::BitXor for Base {
    type Output = Number;
    #[inline]
    fn bitxor(self, other: Self) -> Self::Output {
        (self.value ^ other.value).into()
    }
}

impl ops::Div for Base {
    type Output = Number;
    #[inline]
    fn div(self, other: Self) -> Self::Output {
        if other.value == 0 {
            None
        } else {
            self.value.checked_rem(other.value).and_then(|r| {
                if r == 0 {
                    self.value.checked_div(other.value)
                } else {
                    None
                }
            })
        }
        .map_or_else(|| Rational::from((self, other)).into(), Into::into)
    }
}

impl ops::Mul for Base {
    type Output = Number;
    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        match self.value.overflowing_mul(other.value) {
            (i, false) => Self::new(i).into(),
            (_, true) => Natural::from(self) * other.into(),
        }
    }
}

impl ops::Neg for Base {
    type Output = Number;
    #[inline]
    fn neg(self) -> Self::Output {
        Negative::Base(self).into()
    }
}

impl ops::Not for Base {
    type Output = Number;
    #[inline]
    fn not(self) -> Self::Output {
        (!self.value).into()
    }
}

impl num_traits::pow::Pow<Base> for Base {
    type Output = Number;
    #[inline]
    fn pow(self, other: Self) -> Number {
        if let Ok(value) = other.value.try_into() {
            match self.value.overflowing_pow(value) {
                (i, false) => Self::new(i).into(),
                (_, true) => Natural::from(self).pow(other.into()),
            }
        } else {
            Natural::from(self).pow(other.into())
        }
    }
}

impl ops::Rem for Base {
    type Output = Number;
    #[inline]
    fn rem(self, other: Self) -> Self::Output {
        self.value
            .checked_rem(other.value)
            .map_or_else(|| Number::NaN, Into::into)
    }
}

impl ops::Shl for Base {
    type Output = Number;
    #[inline]
    fn shl(self, other: Self) -> Self::Output {
        (self.value << other.value).into()
    }
}

impl ops::Shr for Base {
    type Output = Number;
    #[inline]
    fn shr(self, other: Self) -> Self::Output {
        (self.value >> other.value).into()
    }
}

impl ops::Sub for Base {
    type Output = Number;
    #[inline]
    fn sub(self, other: Self) -> Self::Output {
        match self.value.overflowing_sub(other.value) {
            (i, false) => i.into(),
            (_, true) => Negative::Base(Self::new(other.value - self.value)).into(),
        }
    }
}

#[allow(clippy::missing_trait_methods)]
impl NumberBase for Base {
    #[allow(clippy::integer_division)]
    #[inline]
    #[must_use]
    fn div_floor(self, other: Self) -> Number {
        (self.value / other.value).into()
    }
    #[inline]
    #[must_use]
    fn gcd(self, other: Self) -> Number {
        let mut a_prime = self.value;
        let mut b_prime = other.value;
        while b_prime > 0 {
            let temp = b_prime;
            b_prime = a_prime % b_prime;
            a_prime = temp;
        }
        a_prime.into()
    }
}
