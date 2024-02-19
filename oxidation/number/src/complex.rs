#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
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
use crate::imag::Imag;
use crate::natural::{Maybe, Natural};
use crate::negative::Negative;
use crate::number::Number;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::fmt_ptr;

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
pub struct Complex {
    real: Imag,
    imag: Imag,
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Complex {
    #[inline]
    fn eq(&self, _other: &u64) -> bool {
        false
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialOrd<u64> for Complex {
    #[inline]
    fn partial_cmp(&self, other: &u64) -> Option<cmp::Ordering> {
        self.real.partial_cmp(other)
    }
}
#[allow(clippy::missing_trait_methods)]
impl PartialOrd<Complex> for Complex {
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering> {
        Some(self.cmp(other))
    }
}

#[allow(clippy::missing_trait_methods)]
impl Ord for Complex {
    #[inline]
    fn cmp(&self, _other: &Self) -> cmp::Ordering {
        cmp::Ordering::Equal
    }
}

impl From<u64> for Complex {
    #[inline]
    fn from(i: u64) -> Self {
        Self {
            real: Imag::Base(Base::new(i)),
            imag: Imag::Base(Base::new(0)),
        }
    }
}
impl From<Base> for Complex {
    #[inline]
    fn from(i: Base) -> Self {
        Self {
            real: Imag::Base(i),
            imag: Imag::Base(Base::new(0)),
        }
    }
}
impl From<Natural> for Complex {
    #[inline]
    fn from(i: Natural) -> Self {
        Self {
            real: match i.reduce() {
                Maybe::Base(v) => Imag::Base(v),
                Maybe::Natural(v) => Imag::Natural(v),
            },
            imag: Imag::Base(Base::new(0)),
        }
    }
}
impl From<Rational> for Complex {
    #[inline]
    fn from(i: Rational) -> Self {
        Self {
            real: Imag::Rational(i),
            imag: Imag::Base(Base::new(0)),
        }
    }
}
impl From<Negative> for Complex {
    #[inline]
    fn from(i: Negative) -> Self {
        Self {
            real: Imag::Negative(i),
            imag: Imag::Base(Base::new(0)),
        }
    }
}
impl From<Imag> for Complex {
    #[inline]
    fn from(i: Imag) -> Self {
        Self {
            real: Imag::Base(Base::new(0)),
            imag: i,
        }
    }
}

impl Complex {
    #[inline]
    #[must_use]
    pub fn new(a: Imag, b: Imag) -> Self {
        Self { real: a, imag: b }
    }
    #[inline]
    #[must_use]
    pub fn imag(self) -> Self {
        Self {
            real: self.imag,
            imag: self.real,
        }
    }
    #[inline]
    #[must_use]
    pub fn reduce(self) -> Number {
        if self.imag == 0 {
            match self.real {
                Imag::Base(a) => a.into(),
                Imag::Natural(a) => a.into(),
                Imag::Rational(a) => a.into(),
                Imag::Negative(a) => a.into(),
            }
        } else if self.real == 0 {
            self.imag.into()
        } else {
            Number::Complex(self)
        }
    }
}

#[allow(clippy::missing_trait_methods)]
impl num_traits::ToPrimitive for Complex {
    #[inline]
    fn to_i64(&self) -> Option<i64> {
        self.real.to_i64()
    }
    #[inline]
    fn to_u64(&self) -> Option<u64> {
        self.real.to_u64()
    }
    #[inline]
    fn to_f64(&self) -> Option<f64> {
        self.real.to_f64()
    }
}

impl Binary for Complex {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        write!(formatter, "{} + {}i", self.real, self.imag)
    }
}

impl Display for Complex {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        write!(formatter, "{} + {}i", self.real, self.imag)
    }
}

impl LowerExp for Complex {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        write!(formatter, "{}", self.real)
    }
}

impl LowerHex for Complex {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        write!(formatter, "{:x} + {:x}i", self.real, self.imag)
    }
}

impl Octal for Complex {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        write!(formatter, "{:o} + {:o}i", self.real, self.imag)
    }
}

impl Pointer for Complex {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        fmt_ptr(self, formatter)
    }
}

impl UpperExp for Complex {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        write!(formatter, "{}", self.real)
    }
}

impl UpperHex for Complex {
    #[inline]
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result {
        write!(formatter, "{:X} + {:X}i", self.real, self.imag)
    }
}

impl Add for Complex {
    type Output = Number;
    #[inline]
    fn add(self, other: Self) -> Self::Output {
        self.real + other.real + (self.imag + other.imag).imag()
    }
}

impl BitAnd for Complex {
    type Output = Number;
    #[inline]
    fn bitand(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl BitOr for Complex {
    type Output = Number;
    #[inline]
    fn bitor(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl BitXor for Complex {
    type Output = Number;
    #[inline]
    fn bitxor(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl Div for Complex {
    type Output = Number;
    #[inline]
    fn div(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl Mul for Complex {
    type Output = Number;
    #[inline]
    fn mul(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl Neg for Complex {
    type Output = Number;
    #[inline]
    fn neg(self) -> Self::Output {
        Number::NaN
    }
}

impl Not for Complex {
    type Output = Number;
    #[inline]
    fn not(self) -> Self::Output {
        Number::NaN
    }
}

impl Pow<Complex> for Complex {
    type Output = Number;
    #[inline]
    fn pow(self, _other: Self) -> Number {
        Number::NaN
    }
}

impl Rem for Complex {
    type Output = Number;
    #[inline]
    fn rem(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl Shl for Complex {
    type Output = Number;
    #[inline]
    fn shl(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl Shr for Complex {
    type Output = Number;
    #[inline]
    fn shr(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl Sub for Complex {
    type Output = Number;
    #[inline]
    fn sub(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

#[allow(clippy::missing_trait_methods)]
impl NumberBase for Complex {
    #[inline]
    #[must_use]
    fn abs(self) -> Number {
        self.real.abs().imag() + self.imag.abs()
    }
    #[inline]
    #[must_use]
    fn div_floor(self, other: Self) -> Number {
        self.real.div_floor(other.real).imag() + self.imag.div_floor(other.imag)
    }
}
