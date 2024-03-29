#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::exhaustive_enums)]
#![allow(clippy::implicit_return)]
#![allow(clippy::min_ident_chars)]
#![allow(clippy::missing_docs_in_private_items)]

use core::cmp;
use core::fmt::{
    Binary, Debug, Display, Formatter, LowerExp, LowerHex, Octal, Pointer, Result, UpperExp,
    UpperHex,
};
use core::ops::{Add, BitAnd, BitOr, BitXor, Div, Mul, Neg, Not, Rem, Shl, Shr, Sub};
use num_traits::Pow;

use crate::base::Base;
use crate::natural::Natural;
use crate::number::Number;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::fmt_ptr;

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
pub enum Negative {
    Base(Base),
    Natural(Natural),
    Rational(Rational),
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Negative {
    #[inline]
    fn eq(&self, _other: &u64) -> bool {
        false
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialOrd<u64> for Negative {
    #[inline]
    fn partial_cmp(&self, _other: &u64) -> Option<cmp::Ordering> {
        Some(cmp::Ordering::Greater)
    }
}
#[allow(clippy::missing_trait_methods)]
impl PartialOrd<Negative> for Negative {
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering> {
        Some(self.cmp(other))
    }
}

#[allow(clippy::missing_trait_methods)]
impl Ord for Negative {
    #[inline]
    fn cmp(&self, other: &Self) -> cmp::Ordering {
        match (self.clone(), other.clone()) {
            (Self::Base(a), Self::Base(b)) => b.cmp(&a),
            (Self::Base(a), Self::Natural(b)) => b.cmp(&a.into()),
            (Self::Base(a), Self::Rational(b)) => b.cmp(&a.into()),
            (Self::Natural(a), Self::Base(b)) => a.cmp(&b.into()).reverse(),
            (Self::Natural(a), Self::Natural(b)) => b.cmp(&a),
            (Self::Natural(a), Self::Rational(b)) => b.cmp(&a.into()),
            (Self::Rational(a), Self::Base(b)) => a.cmp(&b.into()).reverse(),
            (Self::Rational(a), Self::Natural(b)) => a.cmp(&b.into()).reverse(),
            (Self::Rational(a), Self::Rational(b)) => b.cmp(&a),
        }
    }
}

#[allow(clippy::missing_trait_methods)]
impl num_traits::ToPrimitive for Negative {
    #[inline]
    fn to_i64(&self) -> Option<i64> {
        match self.clone() {
            Self::Base(a) => a.to_i64(),
            Self::Natural(a) => a.to_i64(),
            Self::Rational(a) => a.to_i64(),
        }
    }
    #[inline]
    fn to_u64(&self) -> Option<u64> {
        match self.clone() {
            Self::Base(a) => a.to_u64(),
            Self::Natural(a) => a.to_u64(),
            Self::Rational(a) => a.to_u64(),
        }
    }
    #[inline]
    fn to_f64(&self) -> Option<f64> {
        match self.clone() {
            Self::Base(a) => a.to_f64(),
            Self::Natural(a) => a.to_f64(),
            Self::Rational(a) => a.to_f64(),
        }
    }
}

impl Binary for Negative {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        match self.clone() {
            Self::Base(a) => write!(formatter, "-{a}"),
            Self::Natural(a) => write!(formatter, "-{a}"),
            Self::Rational(a) => write!(formatter, "-{a}"),
        }
    }
}

impl Display for Negative {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        match self.clone() {
            Self::Base(a) => write!(formatter, "-{a}"),
            Self::Natural(a) => write!(formatter, "-{a}"),
            Self::Rational(a) => write!(formatter, "-{a}"),
        }
    }
}

impl LowerExp for Negative {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        match self.clone() {
            Self::Base(a) => write!(formatter, "-{a}"),
            Self::Natural(a) => write!(formatter, "-{a}"),
            Self::Rational(a) => write!(formatter, "-{a}"),
        }
    }
}

impl LowerHex for Negative {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        match self.clone() {
            Self::Base(a) => write!(formatter, "-{a}"),
            Self::Natural(a) => write!(formatter, "-{a}"),
            Self::Rational(a) => write!(formatter, "-{a}"),
        }
    }
}

impl Octal for Negative {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        match self.clone() {
            Self::Base(a) => write!(formatter, "-{a}"),
            Self::Natural(a) => write!(formatter, "-{a}"),
            Self::Rational(a) => write!(formatter, "-{a}"),
        }
    }
}

impl Pointer for Negative {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        fmt_ptr(self, formatter)
    }
}

impl UpperExp for Negative {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        match self.clone() {
            Self::Base(a) => write!(formatter, "-{a}"),
            Self::Natural(a) => write!(formatter, "-{a}"),
            Self::Rational(a) => write!(formatter, "-{a}"),
        }
    }
}

impl UpperHex for Negative {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        match self.clone() {
            Self::Base(a) => write!(formatter, "-{a}"),
            Self::Natural(a) => write!(formatter, "-{a}"),
            Self::Rational(a) => write!(formatter, "-{a}"),
        }
    }
}

impl Add for Negative {
    type Output = Number;
    #[inline]
    fn add(self, other: Self) -> Self::Output {
        -match (self, other) {
            (Self::Base(a), Self::Base(b)) => a + b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b + a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b + a.into(),
            (Self::Natural(a), Self::Natural(b)) => a + b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b + a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a + b,
        }
    }
}

impl BitAnd for Negative {
    type Output = Number;
    #[inline]
    fn bitand(self, other: Self) -> Self::Output {
        -match (self, other) {
            (Self::Base(a), Self::Base(b)) => a & b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b & a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b & a.into(),
            (Self::Natural(a), Self::Natural(b)) => a & b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b & a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a & b,
        }
    }
}

impl BitOr for Negative {
    type Output = Number;
    #[inline]
    fn bitor(self, other: Self) -> Self::Output {
        -match (self, other) {
            (Self::Base(a), Self::Base(b)) => a | b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b | a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b | a.into(),
            (Self::Natural(a), Self::Natural(b)) => a | b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b | a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a | b,
        }
    }
}

impl BitXor for Negative {
    type Output = Number;
    #[inline]
    fn bitxor(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a ^ b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b ^ a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b ^ a.into(),
            (Self::Natural(a), Self::Natural(b)) => a ^ b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b ^ a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a ^ b,
        }
    }
}

impl Div for Negative {
    type Output = Number;
    #[inline]
    fn div(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a / b,
            (Self::Base(a), Self::Natural(b)) => Rational::from(a) / b.into(),
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) / b,
            (Self::Natural(a), Self::Base(b)) => Rational::from(a) / b.into(),
            (Self::Natural(a), Self::Natural(b)) => Rational::from(a) / b.into(),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) / b,
            (Self::Rational(a), Self::Base(b)) => a / b.into(),
            (Self::Rational(a), Self::Natural(b)) => a / b.into(),
            (Self::Rational(a), Self::Rational(b)) => a / b,
        }
    }
}

impl Mul for Negative {
    type Output = Number;
    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a * b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b * a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b * a.into(),
            (Self::Natural(a), Self::Natural(b)) => a * b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b * a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a * b,
        }
    }
}

impl Neg for Negative {
    type Output = Number;
    #[inline]
    fn neg(self) -> Self::Output {
        match self {
            Self::Base(a) => a.into(),
            Self::Natural(a) => a.into(),
            Self::Rational(a) => a.into(),
        }
    }
}

impl Not for Negative {
    type Output = Number;
    #[inline]
    fn not(self) -> Self::Output {
        match self {
            Self::Base(a) => !a,
            Self::Natural(a) => a.into(),
            Self::Rational(a) => !a,
        }
    }
}

impl Pow<Negative> for Negative {
    type Output = Number;
    #[inline]
    fn pow(self, _other: Self) -> Number {
        Number::NaN
    }
}

impl Rem for Negative {
    type Output = Number;
    #[inline]
    fn rem(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a % b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) % b,
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) % b,
            (Self::Natural(a), Self::Base(b)) => a % b.into(),
            (Self::Natural(a), Self::Natural(b)) => a % b,
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) % b,
            (Self::Rational(a), Self::Base(b)) => a % b.into(),
            (Self::Rational(a), Self::Natural(b)) => a % b.into(),
            (Self::Rational(a), Self::Rational(b)) => a % b,
        }
    }
}

impl Shl for Negative {
    type Output = Number;
    #[inline]
    fn shl(self, other: Self) -> Self::Output {
        -match (self, other) {
            (Self::Base(a), Self::Base(b)) => a >> b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) >> b,
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) >> b,
            (Self::Natural(a), Self::Base(b)) => a >> b.into(),
            (Self::Natural(a), Self::Natural(b)) => a >> b,
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) >> b,
            (Self::Rational(a), Self::Base(b)) => a >> b.into(),
            (Self::Rational(a), Self::Natural(b)) => a >> b.into(),
            (Self::Rational(a), Self::Rational(b)) => a >> b,
        }
    }
}

impl Shr for Negative {
    type Output = Number;
    #[inline]
    fn shr(self, other: Self) -> Self::Output {
        -match (self, other) {
            (Self::Base(a), Self::Base(b)) => a << b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) << b,
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) << b,
            (Self::Natural(a), Self::Base(b)) => a << b.into(),
            (Self::Natural(a), Self::Natural(b)) => a << b,
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) << b,
            (Self::Rational(a), Self::Base(b)) => a << b.into(),
            (Self::Rational(a), Self::Natural(b)) => a << b.into(),
            (Self::Rational(a), Self::Rational(b)) => a << b,
        }
    }
}

impl Sub for Negative {
    type Output = Number;
    #[inline]
    fn sub(self, other: Self) -> Self::Output {
        -match (self, other) {
            (Self::Base(a), Self::Base(b)) => b - a,
            (Self::Base(a), Self::Natural(b)) => b - a.into(),
            (Self::Base(a), Self::Rational(b)) => b - a.into(),
            (Self::Natural(a), Self::Base(b)) => Natural::from(b) - a,
            (Self::Natural(a), Self::Natural(b)) => b - a,
            (Self::Natural(a), Self::Rational(b)) => b - a.into(),
            (Self::Rational(a), Self::Base(b)) => Rational::from(b) - a,
            (Self::Rational(a), Self::Natural(b)) => Rational::from(b) - a,
            (Self::Rational(a), Self::Rational(b)) => b - a,
        }
    }
}

#[allow(clippy::missing_trait_methods)]
impl NumberBase for Negative {
    #[inline]
    #[must_use]
    fn abs(self) -> Number {
        match self {
            Self::Base(a) => a.into(),
            Self::Natural(a) => a.into(),
            Self::Rational(a) => a.into(),
        }
    }
    #[inline]
    #[must_use]
    fn div_floor(self, other: Self) -> Number {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a.div_floor(b),
            (Self::Base(a), Self::Natural(b)) => Natural::from(a).div_floor(b),
            (Self::Base(a), Self::Rational(b)) => Rational::from(a).div_floor(b),
            (Self::Natural(a), Self::Base(b)) => a.div_floor(b.into()),
            (Self::Natural(a), Self::Natural(b)) => a.div_floor(b),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a).div_floor(b),
            (Self::Rational(a), Self::Base(b)) => a.div_floor(b.into()),
            (Self::Rational(a), Self::Natural(b)) => a.div_floor(b.into()),
            (Self::Rational(a), Self::Rational(b)) => a.div_floor(b),
        }
    }
}
