#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::implicit_return)]
#![allow(clippy::missing_docs_in_private_items)]

use core::cmp;
use core::fmt::{
    Binary, Debug, Display, Formatter, LowerExp, LowerHex, Octal, Pointer, Result, UpperExp,
    UpperHex,
};
use core::ops::{Add, BitAnd, BitOr, BitXor, Div, Mul, Neg, Not, Rem, Shl, Shr, Sub};
use num_traits::Pow;

use crate::natural::Natural;
use crate::negative::Negative;
use crate::number::Number;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::fmt_ptr;

#[derive(Clone, Copy, Debug, Default, Eq, Hash, Ord, PartialEq, PartialOrd)]
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

impl Binary for Base {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter) -> Result {
        Binary::fmt(&self.value, formatter)
    }
}

impl Display for Base {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter) -> Result {
        Display::fmt(&self.value, formatter)
    }
}

impl LowerExp for Base {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter) -> Result {
        LowerExp::fmt(&self.value, formatter)
    }
}

impl LowerHex for Base {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter) -> Result {
        LowerHex::fmt(&self.value, formatter)
    }
}

impl Octal for Base {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter) -> Result {
        Octal::fmt(&self.value, formatter)
    }
}

impl Pointer for Base {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter) -> Result {
        fmt_ptr(self, formatter)
    }
}

impl UpperExp for Base {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter) -> Result {
        UpperExp::fmt(&self.value, formatter)
    }
}

impl UpperHex for Base {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter) -> Result {
        UpperHex::fmt(&self.value, formatter)
    }
}

impl Add for Base {
    type Output = Number;
    #[inline]
    fn add(self, other: Self) -> Self::Output {
        match self.value.overflowing_add(other.value) {
            (i, false) => i.into(),
            (_, true) => Natural::from(self) + other.into(),
        }
    }
}

impl BitAnd for Base {
    type Output = Number;
    #[inline]
    fn bitand(self, other: Self) -> Self::Output {
        (self.value & other.value).into()
    }
}

impl BitOr for Base {
    type Output = Number;
    #[inline]
    fn bitor(self, other: Self) -> Self::Output {
        (self.value | other.value).into()
    }
}

impl BitXor for Base {
    type Output = Number;
    #[inline]
    fn bitxor(self, other: Self) -> Self::Output {
        (self.value ^ other.value).into()
    }
}

impl Div for Base {
    type Output = Number;
    #[inline]
    fn div(self, other: Self) -> Self::Output {
        if other.value == 0 {
            None
        } else {
            self.value.checked_rem(other.value).and_then(|remain| {
                if remain == 0 {
                    self.value.checked_div(other.value)
                } else {
                    None
                }
            })
        }
        .map_or_else(|| Rational::from((self, other)).into(), Into::into)
    }
}

impl Mul for Base {
    type Output = Number;
    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        match self.value.overflowing_mul(other.value) {
            (i, false) => Self::new(i).into(),
            (_, true) => Natural::from(self) * other.into(),
        }
    }
}

impl Neg for Base {
    type Output = Number;
    #[inline]
    fn neg(self) -> Self::Output {
        Negative::Base(self).into()
    }
}

impl Not for Base {
    type Output = Number;
    #[inline]
    fn not(self) -> Self::Output {
        (!self.value).into()
    }
}

impl Pow<Base> for Base {
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

impl Rem for Base {
    type Output = Number;
    #[inline]
    fn rem(self, other: Self) -> Self::Output {
        self.value
            .checked_rem(other.value)
            .map_or_else(|| Number::NaN, Into::into)
    }
}

impl Shl for Base {
    type Output = Number;
    #[inline]
    fn shl(self, other: Self) -> Self::Output {
        (self.value << other.value).into()
    }
}

impl Shr for Base {
    type Output = Number;
    #[inline]
    fn shr(self, other: Self) -> Self::Output {
        (self.value >> other.value).into()
    }
}

impl Sub for Base {
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
