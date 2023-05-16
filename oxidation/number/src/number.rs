#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::separated_literal_suffix)]
#![allow(clippy::implicit_return)]
#![allow(clippy::missing_docs_in_private_items)]

use core::{cmp, fmt, ops};

use crate::base::Base;
use crate::complex::Complex;
use crate::imag::Imag;
use crate::natural::{Maybe, Natural};
use crate::negative::Negative;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::{fmt_ptr, gcd};

#[derive(Clone, Debug, Eq, PartialEq)]
#[non_exhaustive]
pub enum Number {
    Base(Base),
    Natural(Natural),
    Rational(Rational),
    Negative(Negative),
    Imag(Imag),
    Complex(Complex),
    NaN,
}

impl Default for Number {
    #[inline]
    fn default() -> Self {
        0.into()
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Number {
    #[inline]
    fn eq(&self, other: &u64) -> bool {
        match self.clone() {
            Self::Base(a) => a == *other,
            Self::Natural(_)
            | Self::Rational(_)
            | Self::Negative(_)
            | Self::Imag(_)
            | Self::Complex(_)
            | Self::NaN => false,
        }
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialOrd<u64> for Number {
    #[inline]
    fn partial_cmp(&self, other: &u64) -> Option<cmp::Ordering> {
        match self.clone() {
            Self::Base(a) => a.partial_cmp(other),
            Self::Natural(a) => a.partial_cmp(other),
            Self::Rational(a) => a.partial_cmp(other),
            Self::Negative(a) => a.partial_cmp(other),
            Self::Imag(a) => a.partial_cmp(other),
            Self::Complex(a) => a.partial_cmp(other),
            Self::NaN => None,
        }
    }
}
#[allow(clippy::missing_trait_methods)]
impl PartialOrd<Number> for Number {
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering> {
        match (self.clone(), other.clone()) {
            (Self::NaN, _) | (_, Self::NaN) => None,
            (Self::Base(a), Self::Base(b)) => Some(a.cmp(&b)),
            (Self::Base(a), Self::Natural(b)) => Some(b.cmp(&a.into()).reverse()),
            (Self::Base(a), Self::Rational(b)) => Some(b.cmp(&a.into()).reverse()),
            (Self::Base(a), Self::Imag(b)) => Some(Complex::cmp(&a.into(), &b.into())),
            (Self::Base(a), Self::Complex(b)) => Some(b.cmp(&a.into()).reverse()),
            (Self::Natural(a), Self::Base(b)) => Some(a.cmp(&b.into())),
            (Self::Natural(a), Self::Natural(b)) => Some(a.cmp(&b)),
            (Self::Natural(a), Self::Rational(b)) => Some(b.cmp(&a.into()).reverse()),
            (Self::Natural(a), Self::Imag(b)) => Some(Complex::cmp(&a.into(), &b.into())),
            (Self::Natural(a), Self::Complex(b)) => Some(b.cmp(&a.into()).reverse()),
            (Self::Rational(a), Self::Base(b)) => Some(a.cmp(&b.into())),
            (Self::Rational(a), Self::Natural(b)) => Some(a.cmp(&b.into())),
            (Self::Rational(a), Self::Rational(b)) => Some(a.cmp(&b)),
            (Self::Rational(a), Self::Imag(b)) => Some(Complex::cmp(&a.into(), &b.into())),
            (Self::Rational(a), Self::Complex(b)) => Some(b.cmp(&a.into()).reverse()),
            (Self::Negative(a), Self::Negative(b)) => Some(a.cmp(&b)),
            (Self::Negative(a), Self::Imag(b)) => Some(Complex::cmp(&a.into(), &b.into())),
            (Self::Negative(a), Self::Complex(b)) => Some(b.cmp(&a.into()).reverse()),
            (Self::Imag(a), Self::Base(b)) => Some(Complex::cmp(&a.into(), &b.into())),
            (Self::Imag(a), Self::Natural(b)) => Some(Complex::cmp(&a.into(), &b.into())),
            (Self::Imag(a), Self::Rational(b)) => Some(Complex::cmp(&a.into(), &b.into())),
            (Self::Imag(a), Self::Negative(b)) => Some(Complex::cmp(&a.into(), &b.into())),
            (Self::Imag(a), Self::Imag(b)) => Some(a.cmp(&b)),
            (Self::Imag(a), Self::Complex(b)) => Some(b.cmp(&a.into()).reverse()),
            (Self::Complex(a), Self::Base(b)) => Some(a.cmp(&b.into())),
            (Self::Complex(a), Self::Natural(b)) => Some(a.cmp(&b.into())),
            (Self::Complex(a), Self::Rational(b)) => Some(a.cmp(&b.into())),
            (Self::Complex(a), Self::Negative(b)) => Some(a.cmp(&b.into())),
            (Self::Complex(a), Self::Imag(b)) => Some(a.cmp(&b.into())),
            (Self::Complex(a), Self::Complex(b)) => Some(a.cmp(&b)),
            (_, Self::Negative(_)) | (Self::Negative(_), _) => Some(cmp::Ordering::Less),
        }
    }
}

#[allow(clippy::missing_trait_methods)]
impl Ord for Number {
    #[inline]
    fn cmp(&self, other: &Self) -> cmp::Ordering {
        self.partial_cmp(other).unwrap_or(cmp::Ordering::Less)
    }
}

impl From<u64> for Number {
    #[inline]
    fn from(i: u64) -> Self {
        Base::new(i).into()
    }
}
impl From<Base> for Number {
    #[inline]
    fn from(i: Base) -> Self {
        Self::Base(i)
    }
}
impl From<Natural> for Number {
    #[inline]
    fn from(i: Natural) -> Self {
        match i.reduce() {
            Maybe::Base(v) => Self::Base(v),
            Maybe::Natural(v) => Self::Natural(v),
        }
    }
}
impl From<Rational> for Number {
    #[inline]
    fn from(i: Rational) -> Self {
        i.reduce()
    }
}
impl From<Negative> for Number {
    #[inline]
    fn from(i: Negative) -> Self {
        if i == 0 {
            0.into()
        } else {
            Self::Negative(i)
        }
    }
}
impl From<Imag> for Number {
    #[inline]
    fn from(i: Imag) -> Self {
        i.reduce()
    }
}
impl From<Complex> for Number {
    #[inline]
    fn from(i: Complex) -> Self {
        i.reduce()
    }
}

impl Number {
    #[inline]
    #[must_use]
    pub fn new(i: u64) -> Self {
        Base::new(i).into()
    }
    #[inline]
    #[must_use]
    pub fn imag(self) -> Self {
        match self {
            Self::NaN => Self::NaN,
            Self::Base(a) => Imag::Base(a).into(),
            Self::Natural(a) => Imag::Natural(a).into(),
            Self::Rational(a) => Imag::Rational(a).into(),
            Self::Negative(a) => Imag::Negative(a).into(),
            Self::Imag(a) => match a {
                Imag::Base(v) => v.into(),
                Imag::Natural(v) => v.into(),
                Imag::Rational(v) => v.into(),
                Imag::Negative(v) => v.into(),
            },
            Self::Complex(i) => i.imag().into(),
        }
    }
    #[inline]
    #[must_use]
    pub fn is_complex(&self) -> bool {
        matches!(*self, Self::Imag(_) | Self::Complex(_))
    }
    #[inline]
    #[must_use]
    pub fn is_int(&self) -> bool {
        matches!(
            *self,
            Self::Base(_)
                | Self::Natural(_)
                | Self::Negative(Negative::Base(_) | Negative::Natural(_))
        )
    }
    #[inline]
    #[must_use]
    pub fn is_nan(&self) -> bool {
        matches!(*self, Self::NaN)
    }
}

#[allow(clippy::missing_trait_methods)]
impl num_traits::ToPrimitive for Number {
    #[inline]
    fn to_i64(&self) -> Option<i64> {
        match self.clone() {
            Self::Base(a) => a.to_i64(),
            Self::Natural(a) => a.to_i64(),
            Self::Rational(a) => a.to_i64(),
            Self::Negative(a) => a.to_i64(),
            Self::Imag(a) => a.to_i64(),
            Self::Complex(a) => a.to_i64(),
            Self::NaN => None,
        }
    }
    #[inline]
    fn to_u64(&self) -> Option<u64> {
        match self.clone() {
            Self::Base(a) => a.to_u64(),
            Self::Natural(a) => a.to_u64(),
            Self::Rational(a) => a.to_u64(),
            Self::Negative(a) => a.to_u64(),
            Self::Imag(a) => a.to_u64(),
            Self::Complex(a) => a.to_u64(),
            Self::NaN => None,
        }
    }
    #[inline]
    fn to_f64(&self) -> Option<f64> {
        match self.clone() {
            Self::Base(a) => a.to_f64(),
            Self::Natural(a) => a.to_f64(),
            Self::Rational(a) => a.to_f64(),
            Self::Negative(a) => a.to_f64(),
            Self::Imag(a) => a.to_f64(),
            Self::Complex(a) => a.to_f64(),
            Self::NaN => None,
        }
    }
}

impl fmt::Binary for Number {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => fmt::Binary::fmt(&a, f),
            Self::Natural(a) => fmt::Binary::fmt(&a, f),
            Self::Rational(a) => fmt::Binary::fmt(&a, f),
            Self::Negative(a) => fmt::Binary::fmt(&a, f),
            Self::Imag(a) => fmt::Binary::fmt(&a, f),
            Self::Complex(a) => fmt::Binary::fmt(&a, f),
            Self::NaN => fmt::Display::fmt(&"NaN", f),
        }
    }
}

impl fmt::Display for Number {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => a.fmt(f),
            Self::Natural(a) => fmt::Display::fmt(&a, f),
            Self::Rational(a) => a.fmt(f),
            Self::Negative(a) => a.fmt(f),
            Self::Imag(a) => a.fmt(f),
            Self::Complex(a) => a.fmt(f),
            Self::NaN => fmt::Display::fmt(&"NaN", f),
        }
    }
}

impl fmt::LowerExp for Number {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => fmt::LowerExp::fmt(&a, f),
            Self::Natural(a) => fmt::Display::fmt(&a, f),
            Self::Rational(a) => fmt::LowerExp::fmt(&a, f),
            Self::Negative(a) => fmt::LowerExp::fmt(&a, f),
            Self::Imag(a) => fmt::LowerExp::fmt(&a, f),
            Self::Complex(a) => fmt::LowerExp::fmt(&a, f),
            Self::NaN => fmt::Display::fmt(&"NaN", f),
        }
    }
}

impl fmt::LowerHex for Number {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => fmt::LowerHex::fmt(&a, f),
            Self::Natural(a) => fmt::LowerHex::fmt(&a, f),
            Self::Rational(a) => fmt::LowerHex::fmt(&a, f),
            Self::Negative(a) => fmt::LowerHex::fmt(&a, f),
            Self::Imag(a) => fmt::LowerHex::fmt(&a, f),
            Self::Complex(a) => fmt::LowerHex::fmt(&a, f),
            Self::NaN => fmt::Display::fmt(&"NaN", f),
        }
    }
}

impl fmt::Octal for Number {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => fmt::Octal::fmt(&a, f),
            Self::Natural(a) => fmt::Octal::fmt(&a, f),
            Self::Rational(a) => fmt::Octal::fmt(&a, f),
            Self::Negative(a) => fmt::Octal::fmt(&a, f),
            Self::Imag(a) => fmt::Octal::fmt(&a, f),
            Self::Complex(a) => fmt::Octal::fmt(&a, f),
            Self::NaN => fmt::Display::fmt(&"NaN", f),
        }
    }
}

impl fmt::Pointer for Number {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt_ptr(self, f)
    }
}

impl fmt::UpperExp for Number {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => fmt::UpperExp::fmt(&a, f),
            Self::Natural(a) => fmt::Display::fmt(&a, f),
            Self::Rational(a) => fmt::UpperExp::fmt(&a, f),
            Self::Negative(a) => fmt::UpperExp::fmt(&a, f),
            Self::Imag(a) => fmt::UpperExp::fmt(&a, f),
            Self::Complex(a) => fmt::UpperExp::fmt(&a, f),
            Self::NaN => fmt::Display::fmt(&"NaN", f),
        }
    }
}

impl fmt::UpperHex for Number {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => fmt::UpperHex::fmt(&a, f),
            Self::Natural(a) => fmt::UpperHex::fmt(&a, f),
            Self::Rational(a) => fmt::UpperHex::fmt(&a, f),
            Self::Negative(a) => fmt::UpperHex::fmt(&a, f),
            Self::Imag(a) => fmt::UpperHex::fmt(&a, f),
            Self::Complex(a) => fmt::UpperHex::fmt(&a, f),
            Self::NaN => fmt::Display::fmt(&"NaN", f),
        }
    }
}

impl ops::Add for Number {
    type Output = Self;
    #[inline]
    fn add(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a + b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b + a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b + a.into(),
            (Self::Natural(a), Self::Natural(b)) => a + b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b + a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a + b,
            (Self::Negative(a), Self::Negative(b)) => a + b,
            (n, Self::Negative(i)) | (Self::Negative(i), n) => match (i, n) {
                (_, Self::Negative(_) | Self::NaN) => Self::NaN,
                (Negative::Base(a), Self::Base(b)) => b - a,
                (Negative::Base(a), Self::Natural(b)) => b - a.into(),
                (Negative::Natural(b), Self::Base(a)) => -(b - a.into()),
                (Negative::Base(a), Self::Rational(b)) => b - a.into(),
                (Negative::Rational(b), Self::Base(a)) => -(b - a.into()),
                (Negative::Base(a), Self::Imag(b)) => Complex::from(b) - a.into(),
                (Negative::Base(a), Self::Complex(b)) => b - a.into(),
                (Negative::Natural(a), Self::Natural(b)) => b - a,
                (Negative::Natural(a), Self::Rational(b)) => b - a.into(),
                (Negative::Rational(b), Self::Natural(a)) => -(b - a.into()),
                (Negative::Natural(a), Self::Imag(b)) => Complex::from(b) - a.into(),
                (Negative::Natural(a), Self::Complex(b)) => b - a.into(),
                (Negative::Rational(a), Self::Rational(b)) => b - a,
                (Negative::Rational(a), Self::Imag(b)) => Complex::from(b) - a.into(),
                (Negative::Rational(a), Self::Complex(b)) => b - a.into(),
            },
            (Self::Imag(a), Self::Base(b)) | (Self::Base(b), Self::Imag(a)) => {
                Complex::from(a) + b.into()
            }
            (Self::Imag(a), Self::Natural(b)) | (Self::Natural(b), Self::Imag(a)) => {
                Complex::from(a) + b.into()
            }
            (Self::Imag(a), Self::Rational(b)) | (Self::Rational(b), Self::Imag(a)) => {
                Complex::from(a) + b.into()
            }
            (Self::Imag(a), Self::Imag(b)) => a + b,
            (Self::Complex(a), Self::Base(b)) | (Self::Base(b), Self::Complex(a)) => a + b.into(),
            (Self::Complex(a), Self::Natural(b)) | (Self::Natural(b), Self::Complex(a)) => {
                a + b.into()
            }
            (Self::Complex(a), Self::Rational(b)) | (Self::Rational(b), Self::Complex(a)) => {
                a + b.into()
            }
            (Self::Complex(a), Self::Imag(b)) | (Self::Imag(b), Self::Complex(a)) => a + b.into(),
            (Self::Complex(a), Self::Complex(b)) => a + b,
        }
    }
}

impl ops::BitAnd for Number {
    type Output = Self;
    #[inline]
    fn bitand(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Imag(_) | Self::Complex(_) | Self::NaN, _)
            | (_, Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a & b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b & a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b & a.into(),
            (Self::Natural(a), Self::Natural(b)) => a & b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b & a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a & b,
            (Self::Negative(a), Self::Negative(b)) => a & b,
            (n, Self::Negative(i)) | (Self::Negative(i), n) => match (i, n) {
                (_, Self::Negative(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
                (Negative::Base(a), Self::Base(b)) => a & b,
                (Negative::Base(a), Self::Natural(b)) | (Negative::Natural(b), Self::Base(a)) => {
                    b & a.into()
                }
                (Negative::Base(a), Self::Rational(b)) | (Negative::Rational(b), Self::Base(a)) => {
                    b & a.into()
                }
                (Negative::Natural(a), Self::Natural(b)) => a & b,
                (Negative::Natural(a), Self::Rational(b))
                | (Negative::Rational(b), Self::Natural(a)) => b & a.into(),
                (Negative::Rational(a), Self::Rational(b)) => a & b,
            },
        }
    }
}

impl ops::BitOr for Number {
    type Output = Self;
    #[inline]
    fn bitor(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Imag(_) | Self::Complex(_) | Self::NaN, _)
            | (_, Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a | b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b | a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b | a.into(),
            (Self::Natural(a), Self::Natural(b)) => a | b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b | a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a | b,
            (Self::Negative(a), Self::Negative(b)) => a | b,
            (n, Self::Negative(i)) | (Self::Negative(i), n) => -match (i, n) {
                (_, Self::Negative(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
                (Negative::Base(a), Self::Base(b)) => a | b,
                (Negative::Base(a), Self::Natural(b)) | (Negative::Natural(b), Self::Base(a)) => {
                    b | a.into()
                }
                (Negative::Base(a), Self::Rational(b)) | (Negative::Rational(b), Self::Base(a)) => {
                    b | a.into()
                }
                (Negative::Natural(a), Self::Natural(b)) => a | b,
                (Negative::Natural(a), Self::Rational(b))
                | (Negative::Rational(b), Self::Natural(a)) => b | a.into(),
                (Negative::Rational(a), Self::Rational(b)) => a | b,
            },
        }
    }
}

impl ops::BitXor for Number {
    type Output = Self;
    #[inline]
    fn bitxor(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Imag(_) | Self::Complex(_) | Self::NaN, _)
            | (_, Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a ^ b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b ^ a.into(),
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b ^ a.into(),
            (Self::Natural(a), Self::Natural(b)) => a ^ b,
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b ^ a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a ^ b,
            (Self::Negative(a), Self::Negative(b)) => a ^ b,
            (n, Self::Negative(i)) | (Self::Negative(i), n) => -match (i, n) {
                (_, Self::Negative(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
                (Negative::Base(a), Self::Base(b)) => a ^ b,
                (Negative::Base(a), Self::Natural(b)) | (Negative::Natural(b), Self::Base(a)) => {
                    b ^ a.into()
                }
                (Negative::Base(a), Self::Rational(b)) | (Negative::Rational(b), Self::Base(a)) => {
                    b ^ a.into()
                }
                (Negative::Natural(a), Self::Natural(b)) => a ^ b,
                (Negative::Natural(a), Self::Rational(b))
                | (Negative::Rational(b), Self::Natural(a)) => b ^ a.into(),
                (Negative::Rational(a), Self::Rational(b)) => a ^ b,
            },
        }
    }
}

impl ops::Div for Number {
    type Output = Self;
    #[inline]
    fn div(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a / b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) / b,
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) / b,
            (Self::Base(a), Self::Imag(b)) => Complex::from(a) / b.into(),
            (Self::Base(a), Self::Complex(b)) => Complex::from(a) / b,
            (Self::Natural(a), Self::Base(b)) => a / b.into(),
            (Self::Natural(a), Self::Natural(b)) => a / b,
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) / b,
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a) / b.into(),
            (Self::Natural(a), Self::Complex(b)) => Complex::from(a) / b,
            (Self::Rational(a), Self::Base(b)) => a / b.into(),
            (Self::Rational(a), Self::Natural(b)) => a / b.into(),
            (Self::Rational(a), Self::Rational(b)) => a / b,
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a) / b.into(),
            (Self::Rational(a), Self::Complex(b)) => Complex::from(a) / b,
            (Self::Negative(a), Self::Negative(b)) => a / b,
            (n, Self::Negative(i)) => -match (i, n) {
                (_, Self::Negative(_) | Self::NaN) => Self::NaN,
                (Negative::Base(b), Self::Base(a)) => Rational::from(a) / b.into(),
                (Negative::Base(b), Self::Natural(a)) => Rational::from(a) / b.into(),
                (Negative::Base(b), Self::Rational(a)) => a / b.into(),
                (Negative::Base(b), Self::Imag(a)) => Complex::from(a) / b.into(),
                (Negative::Base(b), Self::Complex(a)) => a / b.into(),
                (Negative::Natural(b), Self::Base(a)) => Rational::from(a) / b.into(),
                (Negative::Natural(b), Self::Natural(a)) => Rational::from(a) / b.into(),
                (Negative::Natural(b), Self::Rational(a)) => a / b.into(),
                (Negative::Natural(b), Self::Imag(a)) => Complex::from(a) / b.into(),
                (Negative::Natural(b), Self::Complex(a)) => a / b.into(),
                (Negative::Rational(b), Self::Base(a)) => Rational::from(a) / b,
                (Negative::Rational(b), Self::Natural(a)) => Rational::from(a) / b,
                (Negative::Rational(b), Self::Rational(a)) => a / b,
                (Negative::Rational(b), Self::Imag(a)) => Complex::from(a) / b.into(),
                (Negative::Rational(b), Self::Complex(a)) => a / b.into(),
            },
            (Self::Negative(i), n) => -match (i, n) {
                (_, Self::Negative(_) | Self::NaN) => Self::NaN,
                (Negative::Base(a), Self::Base(b)) => Rational::from(a) / b.into(),
                (Negative::Base(a), Self::Natural(b)) => Rational::from(a) / b.into(),
                (Negative::Base(a), Self::Rational(b)) => Rational::from(a) / b,
                (Negative::Base(a), Self::Imag(b)) => Complex::from(a) / b.into(),
                (Negative::Base(a), Self::Complex(b)) => Complex::from(a) / b,
                (Negative::Natural(a), Self::Base(b)) => Rational::from(a) / b.into(),
                (Negative::Natural(a), Self::Natural(b)) => Rational::from(a) / b.into(),
                (Negative::Natural(a), Self::Rational(b)) => Rational::from(a) / b,
                (Negative::Natural(a), Self::Imag(b)) => Complex::from(a) / b.into(),
                (Negative::Natural(a), Self::Complex(b)) => Complex::from(a) / b,
                (Negative::Rational(a), Self::Base(b)) => a / b.into(),
                (Negative::Rational(a), Self::Natural(b)) => a / b.into(),
                (Negative::Rational(a), Self::Rational(b)) => a / b,
                (Negative::Rational(a), Self::Imag(b)) => Complex::from(a) / b.into(),
                (Negative::Rational(a), Self::Complex(b)) => Complex::from(a) / b,
            },
            (Self::Imag(a), Self::Base(b)) => Complex::from(a) / b.into(),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a) / b.into(),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a) / b.into(),
            (Self::Imag(a), Self::Imag(b)) => a / b,
            (Self::Imag(a), Self::Complex(b)) => Complex::from(a) / b,
            (Self::Complex(a), Self::Base(b)) => a / b.into(),
            (Self::Complex(a), Self::Natural(b)) => a / b.into(),
            (Self::Complex(a), Self::Rational(b)) => a / b.into(),
            (Self::Complex(a), Self::Imag(b)) => a / b.into(),
            (Self::Complex(a), Self::Complex(b)) => a / b,
        }
    }
}

impl ops::Mul for Number {
    type Output = Self;
    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a * b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b * a.into(),
            (Self::Base(a), Self::Imag(b)) | (Self::Imag(b), Self::Base(a)) => {
                Complex::from(a) * b.into()
            }
            (Self::Base(a), Self::Complex(b)) | (Self::Complex(b), Self::Base(a)) => {
                Complex::from(a) * b
            }
            (Self::Natural(a), Self::Natural(b)) => a * b,
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b * a.into(),
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b * a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a * b,
            (Self::Negative(a), Self::Negative(b)) => a * b,
            (n, Self::Negative(i)) | (Self::Negative(i), n) => {
                -match (i, n) {
                    (_, Self::Negative(_) | Self::NaN) => Self::NaN,
                    (Negative::Base(a), Self::Base(b)) => a * b,
                    (Negative::Base(a), Self::Natural(b))
                    | (Negative::Natural(b), Self::Base(a)) => b * a.into(),
                    (Negative::Base(a), Self::Rational(b))
                    | (Negative::Rational(b), Self::Base(a)) => b * a.into(),
                    (Negative::Base(a), Self::Imag(b)) => Complex::from(a) * b.into(),
                    (Negative::Base(a), Self::Complex(b)) => b * a.into(),
                    (Negative::Natural(a), Self::Natural(b)) => a * b,
                    (Negative::Natural(a), Self::Rational(b))
                    | (Negative::Rational(b), Self::Natural(a)) => b * a.into(),
                    (Negative::Natural(a), Self::Imag(b)) => Complex::from(a) * b.into(),
                    (Negative::Natural(a), Self::Complex(b)) => b * a.into(),
                    (Negative::Rational(a), Self::Rational(b)) => a * b,
                    (Negative::Rational(a), Self::Imag(b)) => Complex::from(a) * b.into(),
                    (Negative::Rational(a), Self::Complex(b)) => b * a.into(),
                }
            }
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a) * b.into(),
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a) * b.into(),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a) * b.into(),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a) * b.into(),
            (Self::Imag(a), Self::Imag(b)) => a * b,
            (Self::Natural(a), Self::Complex(b)) => Complex::from(a) * b,
            (Self::Rational(a), Self::Complex(b)) => Complex::from(a) * b,
            (Self::Imag(a), Self::Complex(b)) => Complex::from(a) * b,
            (Self::Complex(a), Self::Natural(b)) => a * b.into(),
            (Self::Complex(a), Self::Rational(b)) => a * b.into(),
            (Self::Complex(a), Self::Imag(b)) => a * b.into(),
            (Self::Complex(a), Self::Complex(b)) => a * b,
        }
    }
}

impl ops::Neg for Number {
    type Output = Self;
    #[inline]
    fn neg(self) -> Self::Output {
        match self {
            Self::Base(a) => -a,
            Self::Natural(a) => Negative::Natural(a).into(),
            Self::Rational(a) => -a,
            Self::Negative(a) => -a,
            Self::Imag(a) => -a,
            Self::Complex(a) => -a,
            Self::NaN => Self::NaN,
        }
    }
}

impl ops::Not for Number {
    type Output = Self;
    #[inline]
    fn not(self) -> Self::Output {
        match self {
            Self::Base(a) => !a,
            Self::Natural(a) => a.into(),
            Self::Rational(a) => !a,
            Self::Negative(a) => !a,
            Self::Imag(a) => !a,
            Self::Complex(a) => !a,
            Self::NaN => Self::NaN,
        }
    }
}

impl num_traits::pow::Pow<Number> for Number {
    type Output = Self;
    #[inline]
    fn pow(self, other: Self) -> Self {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a.pow(b),
            (Self::Base(a), Self::Natural(b)) => Natural::from(a).pow(b),
            (Self::Natural(a), Self::Base(b)) => a.pow(b.into()),
            (Self::Natural(a), Self::Natural(b)) => a.pow(b),
            (Self::Base(a), Self::Rational(b)) => Rational::from(a).pow(b),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a).pow(b),
            (Self::Rational(a), Self::Base(b)) => a.pow(b.into()),
            (Self::Rational(a), Self::Natural(b)) => a.pow(b.into()),
            (Self::Rational(a), Self::Rational(b)) => a.pow(b),
            (Self::Base(a), Self::Negative(i)) => -match i {
                Negative::Base(b) => a.pow(b),
                Negative::Natural(b) => Natural::from(a).pow(b),
                Negative::Rational(b) => Rational::from(a).pow(b),
            },
            (Self::Natural(a), Self::Negative(i)) => -match i {
                Negative::Base(b) => a.pow(b.into()),
                Negative::Natural(b) => a.pow(b),
                Negative::Rational(b) => Rational::from(a).pow(b),
            },
            (Self::Rational(a), Self::Negative(i)) => -match i {
                Negative::Base(b) => a.pow(b.into()),
                Negative::Natural(b) => a.pow(b.into()),
                Negative::Rational(b) => a.pow(b),
            },
            (Self::Negative(i), Self::Base(b)) => match i {
                Negative::Base(a) => b.pow(a),
                Negative::Natural(a) => Natural::from(b).pow(a),
                Negative::Rational(a) => Rational::from(b).pow(a),
            },
            (Self::Negative(i), Self::Natural(b)) => match i {
                Negative::Base(a) => b.pow(a.into()),
                Negative::Natural(a) => b.pow(a),
                Negative::Rational(a) => Rational::from(b).pow(a),
            },
            (Self::Negative(i), Self::Rational(b)) => match i {
                Negative::Base(a) => b.pow(a.into()),
                Negative::Natural(a) => b.pow(a.into()),
                Negative::Rational(a) => b.pow(a),
            },
            (Self::Negative(a), Self::Negative(b)) => a.pow(b),
            (Self::Base(a), Self::Imag(b)) => Complex::from(a).pow(b.into()),
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a).pow(b.into()),
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a).pow(b.into()),
            (Self::Negative(a), Self::Imag(b)) => Complex::from(a).pow(b.into()),
            (Self::Imag(a), Self::Base(b)) => Complex::from(a).pow(b.into()),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a).pow(b.into()),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a).pow(b.into()),
            (Self::Imag(a), Self::Negative(b)) => Complex::from(a).pow(b.into()),
            (Self::Imag(a), Self::Imag(b)) => a.pow(b),
            (Self::Base(a), Self::Complex(b)) => Complex::from(a).pow(b),
            (Self::Natural(a), Self::Complex(b)) => Complex::from(a).pow(b),
            (Self::Rational(a), Self::Complex(b)) => Complex::from(a).pow(b),
            (Self::Negative(a), Self::Complex(b)) => Complex::from(a).pow(b),
            (Self::Imag(a), Self::Complex(b)) => Complex::from(a).pow(b),
            (Self::Complex(a), Self::Base(b)) => a.pow(b.into()),
            (Self::Complex(a), Self::Natural(b)) => a.pow(b.into()),
            (Self::Complex(a), Self::Rational(b)) => a.pow(b.into()),
            (Self::Complex(a), Self::Negative(b)) => a.pow(b.into()),
            (Self::Complex(a), Self::Imag(b)) => a.pow(b.into()),
            (Self::Complex(a), Self::Complex(b)) => a.pow(b),
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
        }
    }
}

impl ops::Rem for Number {
    type Output = Self;
    #[inline]
    fn rem(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a % b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) % b,
            (Self::Natural(a), Self::Base(b)) => a % b.into(),
            (Self::Natural(a), Self::Natural(b)) => a % b,
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) % b,
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) % b,
            (Self::Rational(a), Self::Base(b)) => a % b.into(),
            (Self::Rational(a), Self::Natural(b)) => a % b.into(),
            (Self::Rational(a), Self::Rational(b)) => a % b,
            (Self::Base(a), Self::Negative(i)) => -match i {
                Negative::Base(b) => a % b,
                Negative::Natural(b) => Natural::from(a) % b,
                Negative::Rational(b) => Rational::from(a) % b,
            },
            (Self::Natural(a), Self::Negative(i)) => -match i {
                Negative::Base(b) => a % b.into(),
                Negative::Natural(b) => a % b,
                Negative::Rational(b) => Rational::from(a) % b,
            },
            (Self::Rational(a), Self::Negative(i)) => -match i {
                Negative::Base(b) => a % b.into(),
                Negative::Natural(b) => a % b.into(),
                Negative::Rational(b) => a % b,
            },
            (Self::Negative(i), Self::Base(b)) => -match i {
                Negative::Base(a) => a % b,
                Negative::Natural(a) => a % b.into(),
                Negative::Rational(a) => a % b.into(),
            },
            (Self::Negative(i), Self::Natural(b)) => -match i {
                Negative::Base(a) => Natural::from(a) % b,
                Negative::Natural(a) => a % b,
                Negative::Rational(a) => a % b.into(),
            },
            (Self::Negative(i), Self::Rational(b)) => -match i {
                Negative::Base(a) => Rational::from(a) % b,
                Negative::Natural(a) => Rational::from(a) % b,
                Negative::Rational(a) => a % b,
            },
            (Self::Negative(a), Self::Negative(b)) => a % b,
            (Self::Base(a), Self::Imag(b)) => Complex::from(a) % b.into(),
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a) % b.into(),
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a) % b.into(),
            (Self::Negative(a), Self::Imag(b)) => Complex::from(a) % b.into(),
            (Self::Imag(a), Self::Base(b)) => Complex::from(a) % b.into(),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a) % b.into(),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a) % b.into(),
            (Self::Imag(a), Self::Negative(b)) => Complex::from(a) % b.into(),
            (Self::Imag(a), Self::Imag(b)) => a % b,
            (Self::Base(a), Self::Complex(b)) => Complex::from(a) % b,
            (Self::Natural(a), Self::Complex(b)) => Complex::from(a) % b,
            (Self::Rational(a), Self::Complex(b)) => Complex::from(a) % b,
            (Self::Negative(a), Self::Complex(b)) => Complex::from(a) % b,
            (Self::Imag(a), Self::Complex(b)) => Complex::from(a) % b,
            (Self::Complex(a), Self::Base(b)) => a % b.into(),
            (Self::Complex(a), Self::Natural(b)) => a % b.into(),
            (Self::Complex(a), Self::Rational(b)) => a % b.into(),
            (Self::Complex(a), Self::Negative(b)) => a % b.into(),
            (Self::Complex(a), Self::Imag(b)) => a % b.into(),
            (Self::Complex(a), Self::Complex(b)) => a % b,
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
        }
    }
}

impl ops::Shl for Number {
    type Output = Self;
    #[inline]
    fn shl(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Rational(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN, _)
            | (_, Self::Rational(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a << b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) << b,
            (Self::Natural(a), Self::Base(b)) => a << b.into(),
            (Self::Natural(a), Self::Natural(b)) => a << b,
            (Self::Negative(a), Self::Negative(b)) => a << b,
            (n, Self::Negative(i)) => match (i, n) {
                (Negative::Base(b), Self::Base(a)) => a >> b,
                (Negative::Base(b), Self::Natural(a)) => a >> b.into(),
                (Negative::Natural(b), Self::Base(a)) => Natural::from(a) >> b,
                (Negative::Natural(b), Self::Natural(a)) => a >> b,
                (Negative::Rational(_), _)
                | (
                    _,
                    Self::Negative(_)
                    | Self::Rational(_)
                    | Self::Imag(_)
                    | Self::Complex(_)
                    | Self::NaN,
                ) => Self::NaN,
            },
            (Self::Negative(i), n) => -match (i, n) {
                (Negative::Base(a), Self::Base(b)) => a << b,
                (Negative::Base(a), Self::Natural(b)) => Natural::from(a) << b,
                (Negative::Natural(a), Self::Base(b)) => a << b.into(),
                (Negative::Natural(a), Self::Natural(b)) => a << b,
                (Negative::Rational(_), _)
                | (
                    _,
                    Self::Negative(_)
                    | Self::Rational(_)
                    | Self::Imag(_)
                    | Self::Complex(_)
                    | Self::NaN,
                ) => Self::NaN,
            },
        }
    }
}

impl ops::Shr for Number {
    type Output = Self;
    #[inline]
    fn shr(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Rational(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN, _)
            | (_, Self::Rational(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a >> b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) >> b,
            (Self::Natural(a), Self::Base(b)) => a >> b.into(),
            (Self::Natural(a), Self::Natural(b)) => a >> b,
            (Self::Negative(a), Self::Negative(b)) => a >> b,
            (n, Self::Negative(i)) => match (i, n) {
                (Negative::Base(b), Self::Base(a)) => a << b,
                (Negative::Base(b), Self::Natural(a)) => a << b.into(),
                (Negative::Natural(b), Self::Base(a)) => Natural::from(a) << b,
                (Negative::Natural(b), Self::Natural(a)) => a << b,
                (Negative::Rational(_), _)
                | (
                    _,
                    Self::Negative(_)
                    | Self::Rational(_)
                    | Self::Imag(_)
                    | Self::Complex(_)
                    | Self::NaN,
                ) => Self::NaN,
            },
            (Self::Negative(i), n) => -match (i, n) {
                (Negative::Base(a), Self::Base(b)) => a >> b,
                (Negative::Base(a), Self::Natural(b)) => Natural::from(a) >> b,
                (Negative::Natural(a), Self::Base(b)) => a >> b.into(),
                (Negative::Natural(a), Self::Natural(b)) => a >> b,
                (Negative::Rational(_), _)
                | (
                    _,
                    Self::Negative(_)
                    | Self::Rational(_)
                    | Self::Imag(_)
                    | Self::Complex(_)
                    | Self::NaN,
                ) => Self::NaN,
            },
        }
    }
}

impl ops::Sub for Number {
    type Output = Self;
    #[inline]
    fn sub(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a - b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) - b,
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) - b,
            (Self::Natural(a), Self::Base(b)) => a - b.into(),
            (Self::Natural(a), Self::Natural(b)) => a - b,
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) - b,
            (Self::Rational(a), Self::Base(b)) => a - b.into(),
            (Self::Rational(a), Self::Natural(b)) => a - b.into(),
            (Self::Rational(a), Self::Rational(b)) => a - b,
            (Self::Base(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a + b,
                Negative::Natural(b) => Natural::from(a) + b,
                Negative::Rational(b) => Rational::from(a) + b,
            },
            (Self::Natural(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a + b.into(),
                Negative::Natural(b) => a + b,
                Negative::Rational(b) => Rational::from(a) + b,
            },
            (Self::Rational(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a + b.into(),
                Negative::Natural(b) => a + b.into(),
                Negative::Rational(b) => a + b,
            },
            (Self::Negative(i), Self::Base(b)) => -match i {
                Negative::Base(a) => a + b,
                Negative::Natural(a) => a + b.into(),
                Negative::Rational(a) => a + b.into(),
            },
            (Self::Negative(i), Self::Natural(b)) => -match i {
                Negative::Base(a) => Natural::from(a) + b,
                Negative::Natural(a) => a + b,
                Negative::Rational(a) => a + b.into(),
            },
            (Self::Negative(i), Self::Rational(b)) => -match i {
                Negative::Base(a) => Rational::from(a) + b,
                Negative::Natural(a) => Rational::from(a) + b,
                Negative::Rational(a) => a + b,
            },
            (Self::Negative(a), Self::Negative(b)) => a - b,
            (Self::Base(a), Self::Imag(b)) => Complex::from(a) - b.into(),
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a) - b.into(),
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a) - b.into(),
            (Self::Negative(a), Self::Imag(b)) => Complex::from(a) - b.into(),
            (Self::Imag(a), Self::Base(b)) => Complex::from(a) - b.into(),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a) - b.into(),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a) - b.into(),
            (Self::Imag(a), Self::Negative(b)) => Complex::from(a) - b.into(),
            (Self::Imag(a), Self::Imag(b)) => a - b,
            (Self::Base(a), Self::Complex(b)) => -(b - a.into()),
            (Self::Natural(a), Self::Complex(b)) => -(b - a.into()),
            (Self::Rational(a), Self::Complex(b)) => -(b - a.into()),
            (Self::Negative(a), Self::Complex(b)) => -(b - a.into()),
            (Self::Imag(a), Self::Complex(b)) => -(b - a.into()),
            (Self::Complex(a), Self::Base(b)) => a - b.into(),
            (Self::Complex(a), Self::Natural(b)) => a - b.into(),
            (Self::Complex(a), Self::Rational(b)) => a - b.into(),
            (Self::Complex(a), Self::Negative(b)) => a - b.into(),
            (Self::Complex(a), Self::Imag(b)) => a - b.into(),
            (Self::Complex(a), Self::Complex(b)) => a - b,
        }
    }
}

#[allow(clippy::missing_trait_methods)]
impl NumberBase for Number {
    #[inline]
    #[must_use]
    fn abs(self) -> Self {
        match self {
            Self::Base(a) => a.abs(),
            Self::Natural(a) => a.into(),
            Self::Rational(a) => a.abs(),
            Self::Negative(a) => a.abs(),
            Self::Imag(a) => a.abs(),
            Self::Complex(a) => a.abs(),
            Self::NaN => Self::NaN,
        }
    }
    #[inline]
    #[must_use]
    fn div_floor(self, other: Self) -> Self {
        match (self, other) {
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a.div_floor(b),
            (Self::Base(a), Self::Natural(b)) => Natural::from(a).div_floor(b),
            (Self::Base(a), Self::Rational(b)) => Rational::from(a).div_floor(b),
            (Self::Natural(a), Self::Base(b)) => a.div_floor(b.into()),
            (Self::Natural(a), Self::Natural(b)) => a.div_floor(b),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a).div_floor(b),
            (Self::Rational(a), Self::Base(b)) => a.div_floor(b.into()),
            (Self::Rational(a), Self::Natural(b)) => a.div_floor(b.into()),
            (Self::Rational(a), Self::Rational(b)) => a.div_floor(b),
            (Self::Negative(a), Self::Negative(b)) => a.div_floor(b),
            (Self::Base(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a.div_floor(b),
                Negative::Natural(b) => Natural::from(a).div_floor(b),
                Negative::Rational(b) => Rational::from(a).div_floor(b),
            },
            (Self::Natural(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a.div_floor(b.into()),
                Negative::Natural(b) => a.div_floor(b),
                Negative::Rational(b) => Rational::from(a).div_floor(b),
            },
            (Self::Rational(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a.div_floor(b.into()),
                Negative::Natural(b) => a.div_floor(b.into()),
                Negative::Rational(b) => a - b,
            },
            (Self::Negative(i), Self::Base(b)) => -match i {
                Negative::Base(a) => a.div_floor(b),
                Negative::Natural(a) => a.div_floor(b.into()),
                Negative::Rational(a) => a.div_floor(b.into()),
            },
            (Self::Negative(i), Self::Natural(b)) => -match i {
                Negative::Base(a) => Natural::from(a).div_floor(b),
                Negative::Natural(a) => a.div_floor(b),
                Negative::Rational(a) => a.div_floor(b.into()),
            },
            (Self::Negative(i), Self::Rational(b)) => -match i {
                Negative::Base(a) => Rational::from(a).div_floor(b),
                Negative::Natural(a) => Rational::from(a).div_floor(b),
                Negative::Rational(a) => a.div_floor(b),
            },
            (Self::Base(a), Self::Imag(b)) => Complex::from(a).div_floor(b.into()),
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a).div_floor(b.into()),
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a).div_floor(b.into()),
            (Self::Negative(a), Self::Imag(b)) => Complex::from(a).div_floor(b.into()),
            (Self::Imag(a), Self::Base(b)) => Complex::from(a).div_floor(b.into()),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a).div_floor(b.into()),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a).div_floor(b.into()),
            (Self::Imag(a), Self::Negative(b)) => Complex::from(a).div_floor(b.into()),
            (Self::Imag(a), Self::Imag(b)) => a.div_floor(b),
            (Self::Base(a), Self::Complex(b)) => Complex::from(a).div_floor(b),
            (Self::Natural(a), Self::Complex(b)) => Complex::from(a).div_floor(b),
            (Self::Rational(a), Self::Complex(b)) => Complex::from(a).div_floor(b),
            (Self::Negative(a), Self::Complex(b)) => Complex::from(a).div_floor(b),
            (Self::Imag(a), Self::Complex(b)) => Complex::from(a).div_floor(b),
            (Self::Complex(a), Self::Base(b)) => a.div_floor(b.into()),
            (Self::Complex(a), Self::Natural(b)) => a.div_floor(b.into()),
            (Self::Complex(a), Self::Rational(b)) => a.div_floor(b.into()),
            (Self::Complex(a), Self::Negative(b)) => a.div_floor(b.into()),
            (Self::Complex(a), Self::Imag(b)) => a.div_floor(b.into()),
            (Self::Complex(a), Self::Complex(b)) => a.div_floor(b),
        }
    }
    #[inline]
    #[must_use]
    fn gcd(self, other: Self) -> Self {
        match (self, other) {
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a.gcd(b),
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => {
                gcd(a.into(), b)
            }
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => {
                Rational::from(a).gcd(b)
            }
            (Self::Natural(a), Self::Natural(b)) => gcd(a, b),
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                Rational::from(a).gcd(b)
            }
            (Self::Rational(a), Self::Rational(b)) => a.gcd(b),
            (Self::Negative(a), Self::Negative(b)) => a.gcd(b),
            (n, Self::Negative(i)) | (Self::Negative(i), n) => {
                match (i, n) {
                    (_, Self::Negative(_) | Self::NaN) => Self::NaN,
                    (Negative::Base(a), Self::Base(b)) => a.gcd(b),
                    (Negative::Base(a), Self::Natural(b))
                    | (Negative::Natural(b), Self::Base(a)) => gcd(a.into(), b),
                    (Negative::Base(a), Self::Rational(b))
                    | (Negative::Rational(b), Self::Base(a)) => b.gcd(a.into()),
                    (Negative::Base(a), Self::Imag(b)) => Complex::from(a).gcd(b.into()),
                    (Negative::Base(a), Self::Complex(b)) => b.gcd(a.into()),
                    (Negative::Natural(a), Self::Natural(b)) => gcd(a, b),
                    (Negative::Natural(a), Self::Rational(b))
                    | (Negative::Rational(b), Self::Natural(a)) => b.gcd(a.into()),
                    (Negative::Natural(a), Self::Imag(b)) => Complex::from(a).gcd(b.into()),
                    (Negative::Natural(a), Self::Complex(b)) => b.gcd(a.into()),
                    (Negative::Rational(a), Self::Rational(b)) => a.gcd(b),
                    (Negative::Rational(a), Self::Imag(b)) => Complex::from(a).gcd(b.into()),
                    (Negative::Rational(a), Self::Complex(b)) => b.gcd(a.into()),
                }
            }
            (Self::Base(a), Self::Imag(b)) => Complex::from(a).gcd(b.into()),
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a).gcd(b.into()),
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a).gcd(b.into()),
            (Self::Imag(a), Self::Base(b)) => Complex::from(a).gcd(b.into()),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a).gcd(b.into()),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a).gcd(b.into()),
            (Self::Imag(a), Self::Imag(b)) => a.gcd(b),
            (Self::Base(a), Self::Complex(b)) => Complex::from(a).gcd(b),
            (Self::Natural(a), Self::Complex(b)) => Complex::from(a).gcd(b),
            (Self::Rational(a), Self::Complex(b)) => Complex::from(a).gcd(b),
            (Self::Imag(a), Self::Complex(b)) => Complex::from(a).gcd(b),
            (Self::Complex(a), Self::Base(b)) => a.gcd(b.into()),
            (Self::Complex(a), Self::Natural(b)) => a.gcd(b.into()),
            (Self::Complex(a), Self::Rational(b)) => a.gcd(b.into()),
            (Self::Complex(a), Self::Imag(b)) => a.gcd(b.into()),
            (Self::Complex(a), Self::Complex(b)) => a.gcd(b),
        }
    }
}
