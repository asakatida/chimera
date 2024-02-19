#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::exhaustive_enums)]
#![allow(clippy::implicit_return)]
#![allow(clippy::missing_docs_in_private_items)]

use crate::base::Base;
use crate::negative::Negative;
use crate::number::Number;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::fmt_ptr;
use core::{cmp, fmt, ops};
use num_integer::Integer;
use num_traits::Pow;
use num_traits::{CheckedSub, ToPrimitive, Zero};

pub enum Maybe {
    Base(Base),
    Natural(Natural),
}

#[derive(Clone, Debug, Default, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct Natural {
    value: num_bigint::BigUint,
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Natural {
    #[inline]
    fn eq(&self, other: &u64) -> bool {
        self.value == (*other).into()
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialOrd<u64> for Natural {
    #[inline]
    fn partial_cmp(&self, other: &u64) -> Option<cmp::Ordering> {
        self.value.partial_cmp(&(*other).into())
    }
}

impl From<u64> for Natural {
    #[inline]
    fn from(i: u64) -> Self {
        Self { value: i.into() }
    }
}

impl From<Base> for Natural {
    #[inline]
    fn from(i: Base) -> Self {
        Self {
            value: i.to_u64().unwrap_or_default().into(),
        }
    }
}

impl Natural {
    #[inline]
    #[must_use]
    pub fn new(i: num_bigint::BigUint) -> Self {
        Self { value: i }
    }
    #[inline]
    #[must_use]
    pub fn reduce(&self) -> Maybe {
        let mut value = self.value.to_u64_digits();
        if value.len() < 2 {
            Maybe::Base(Base::new(value.pop().unwrap_or_default()))
        } else {
            Maybe::Natural(self.clone())
        }
    }
}

#[allow(clippy::missing_trait_methods)]
impl num_traits::ToPrimitive for Natural {
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

impl fmt::Binary for Natural {
    #[inline]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        fmt::Binary::fmt(&self.value, formatter)
    }
}

impl fmt::Display for Natural {
    #[inline]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        fmt::Display::fmt(&self.value, formatter)
    }
}

impl fmt::LowerExp for Natural {
    #[inline]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        fmt::Display::fmt(&self.value, formatter)
    }
}

impl fmt::LowerHex for Natural {
    #[inline]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        fmt::LowerHex::fmt(&self.value, formatter)
    }
}

impl fmt::Octal for Natural {
    #[inline]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        fmt::Octal::fmt(&self.value, formatter)
    }
}

impl fmt::Pointer for Natural {
    #[inline]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        fmt_ptr(self, formatter)
    }
}

impl fmt::UpperExp for Natural {
    #[inline]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        fmt::Display::fmt(&self.value, formatter)
    }
}

impl fmt::UpperHex for Natural {
    #[inline]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        fmt::UpperHex::fmt(&self.value, formatter)
    }
}

impl ops::Add for Natural {
    type Output = Number;
    #[inline]
    fn add(self, other: Self) -> Self::Output {
        Self::new(self.value + other.value).into()
    }
}

impl ops::BitAnd for Natural {
    type Output = Number;
    #[inline]
    fn bitand(self, other: Self) -> Self::Output {
        Self::new(self.value & other.value).into()
    }
}

impl ops::BitOr for Natural {
    type Output = Number;
    #[inline]
    fn bitor(self, other: Self) -> Self::Output {
        Self::new(self.value | other.value).into()
    }
}

impl ops::BitXor for Natural {
    type Output = Number;
    #[inline]
    fn bitxor(self, other: Self) -> Self::Output {
        Self::new(self.value ^ other.value).into()
    }
}

impl ops::Div for Natural {
    type Output = Number;
    #[inline]
    fn div(self, other: Self) -> Self::Output {
        if other.value.is_zero() {
            None
        } else if (self.value.clone() % other.value.clone()).is_zero() {
            Some(self.value.clone() / other.value.clone())
        } else {
            None
        }
        .map_or_else(
            || Rational::from((self, other)).into(),
            |value| Self::new(value).into(),
        )
    }
}

impl ops::Mul for Natural {
    type Output = Number;
    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        Self::new(self.value * other.value).into()
    }
}

impl ops::Neg for Natural {
    type Output = Number;
    #[inline]
    fn neg(self) -> Self::Output {
        Negative::Natural(self).into()
    }
}

impl ops::Not for Natural {
    type Output = Number;
    #[inline]
    fn not(self) -> Self::Output {
        Self::new(num_bigint::BigUint::new(
            self.value.iter_u32_digits().map(|x| !x).collect(),
        ))
        .into()
    }
}

impl Pow<Natural> for Natural {
    type Output = Number;
    #[inline]
    fn pow(self, _other: Self) -> Number {
        // if self.value.bits() < 32 && other.value.bits() < 5 {
        //     Self::new(self.value.pow(other.value)).into()
        // } else {
        Number::NaN
        // }
    }
}

impl ops::Rem for Natural {
    type Output = Number;
    #[inline]
    fn rem(self, other: Self) -> Self::Output {
        Self::new(self.value % other.value).into()
    }
}

impl ops::Shl for Natural {
    type Output = Number;
    #[inline]
    fn shl(self, other: Self) -> Self::Output {
        other
            .value
            .try_into()
            .map(|x: usize| Self::new(self.value << x).into())
            .unwrap_or(Number::NaN)
    }
}

impl ops::Shr for Natural {
    type Output = Number;
    #[inline]
    fn shr(self, other: Self) -> Self::Output {
        other
            .value
            .try_into()
            .map(|x: usize| Self::new(self.value >> x).into())
            .unwrap_or(Number::NaN)
    }
}

impl ops::Sub for Natural {
    type Output = Number;
    #[inline]
    fn sub(self, other: Self) -> Self::Output {
        self.value
            .checked_sub(&other.value)
            .map(Self::new)
            .map_or_else(|| -Self::new(other.value - self.value), Into::into)
    }
}

#[allow(clippy::missing_trait_methods)]
impl NumberBase for Natural {
    #[inline]
    fn div_floor(self, other: Self) -> Number {
        Self::new(self.value.div_floor(&other.value)).into()
    }
}
