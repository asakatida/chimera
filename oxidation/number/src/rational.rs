#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::float_arithmetic)]
#![allow(clippy::implicit_return)]
#![allow(clippy::integer_arithmetic)]
#![allow(clippy::missing_docs_in_private_items)]

use core::{cmp, fmt, ops};

use crate::base::Base;
use crate::natural::{Maybe, Natural};
use crate::negative::Negative;
use crate::number::Number;
use crate::traits::NumberBase;
use crate::utils::{fmt_ptr, rem};

#[non_exhaustive]
#[derive(Clone, Debug, Eq, PartialEq)]
pub enum Part {
    Base(Base),
    Natural(Natural),
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Part {
    #[inline]
    fn eq(&self, other: &u64) -> bool {
        match self.clone() {
            Self::Base(a) => a == *other,
            Self::Natural(a) => a == *other,
        }
    }
}

impl From<u64> for Part {
    #[inline]
    fn from(i: u64) -> Self {
        Self::Base(Base::new(i))
    }
}
impl From<Base> for Part {
    #[inline]
    fn from(i: Base) -> Self {
        Self::Base(i)
    }
}
impl From<Natural> for Part {
    #[inline]
    fn from(i: Natural) -> Self {
        match i.reduce() {
            Maybe::Base(v) => Self::Base(v),
            Maybe::Natural(v) => Self::Natural(v),
        }
    }
}
impl TryFrom<Number> for Part {
    type Error = &'static str;
    #[inline]
    fn try_from(i: Number) -> Result<Self, Self::Error> {
        match i {
            Number::Base(v) => Ok(Self::Base(v)),
            Number::Natural(v) => Ok(Self::Natural(v)),
            Number::Rational(_)
            | Number::Negative(_)
            | Number::Imag(_)
            | Number::Complex(_)
            | Number::NaN => Err("invalid number type"),
        }
    }
}

impl From<Part> for Number {
    #[inline]
    fn from(part: Part) -> Self {
        match part {
            Part::Base(a) => Self::from(a),
            Part::Natural(a) => Self::from(a),
        }
    }
}

#[allow(clippy::integer_division)]
#[allow(clippy::missing_trait_methods)]
impl num_traits::ToPrimitive for Part {
    #[inline]
    fn to_i64(&self) -> Option<i64> {
        match self.clone() {
            Self::Base(a) => a.to_i64(),
            Self::Natural(a) => a.to_i64(),
        }
    }
    #[inline]
    fn to_u64(&self) -> Option<u64> {
        match self.clone() {
            Self::Base(a) => a.to_u64(),
            Self::Natural(a) => a.to_u64(),
        }
    }
    #[inline]
    fn to_f64(&self) -> Option<f64> {
        match self.clone() {
            Self::Base(a) => a.to_f64(),
            Self::Natural(a) => a.to_f64(),
        }
    }
}

impl ops::Mul for Part {
    type Output = Number;
    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a * b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => b * a.into(),
            (Self::Natural(a), Self::Natural(b)) => a * b,
        }
    }
}

impl Part {
    #[inline]
    #[must_use]
    fn div_floor(self, other: Self) -> Number {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a.div_floor(b),
            (Self::Base(a), Self::Natural(b)) => Natural::from(a).div_floor(b),
            (Self::Natural(a), Self::Base(b)) => a.div_floor(b.into()),
            (Self::Natural(a), Self::Natural(b)) => a.div_floor(b),
        }
    }
    #[inline]
    #[must_use]
    fn gcd(self, other: Self) -> Number {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a.gcd(b),
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => {
                b.gcd(a.into())
            }
            (Self::Natural(a), Self::Natural(b)) => a.gcd(b),
        }
    }
}

impl fmt::Display for Part {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self.clone() {
            Self::Base(n) => write!(f, "{n}"),
            Self::Natural(n) => write!(f, "{n}"),
        }
    }
}

#[non_exhaustive]
#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Rational {
    numerator: Part,
    denominator: Part,
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Rational {
    #[inline]
    fn eq(&self, _other: &u64) -> bool {
        false
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialOrd<u64> for Rational {
    #[inline]
    fn partial_cmp(&self, other: &u64) -> Option<cmp::Ordering> {
        Some(
            Number::from(self.numerator.clone()).cmp(&(self.denominator.clone() * (*other).into())),
        )
    }
}
#[allow(clippy::missing_trait_methods)]
impl PartialOrd<Rational> for Rational {
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering> {
        Some(self.cmp(other))
    }
}

#[allow(clippy::missing_trait_methods)]
impl Ord for Rational {
    #[inline]
    fn cmp(&self, other: &Self) -> cmp::Ordering {
        (self.numerator.clone() * other.denominator.clone())
            .cmp(&(other.numerator.clone() * self.denominator.clone()))
    }
}

impl<T: Into<Part>> From<T> for Rational {
    #[inline]
    fn from(i: T) -> Self {
        Self {
            numerator: i.into(),
            denominator: 1.into(),
        }
    }
}
impl<T: Into<Part>, U: Into<Part>> From<(T, U)> for Rational {
    #[inline]
    fn from(i: (T, U)) -> Self {
        Self {
            numerator: i.0.into(),
            denominator: i.1.into(),
        }
    }
}

impl Rational {
    #[inline]
    #[must_use]
    pub fn reduce(self) -> Number {
        if self.denominator == 0 {
            Number::Rational(self)
        } else if self.denominator == 1 {
            self.numerator.into()
        } else {
            let gcd = self.numerator.clone().gcd(self.denominator.clone());
            if gcd == 1 {
                Number::Rational(self)
            } else if gcd == Number::from(self.denominator.clone()) {
                self.numerator.into()
            } else {
                Part::try_from(Number::from(self.numerator).div_floor(gcd.clone()))
                    .and_then(|n| {
                        Part::try_from(Number::from(self.denominator).div_floor(gcd))
                            .map(|d| (n, d))
                    })
                    .map_or_else(|_| Number::NaN, |x| Number::Rational(Self::from(x)))
            }
        }
    }
}

#[allow(clippy::integer_division)]
#[allow(clippy::missing_trait_methods)]
impl num_traits::ToPrimitive for Rational {
    #[inline]
    fn to_i64(&self) -> Option<i64> {
        self.numerator
            .to_i64()
            .and_then(|x| self.denominator.to_i64().map(|y| x / y))
    }
    #[inline]
    fn to_u64(&self) -> Option<u64> {
        self.numerator
            .to_u64()
            .and_then(|x| self.denominator.to_u64().map(|y| x / y))
    }
    #[inline]
    fn to_f64(&self) -> Option<f64> {
        self.numerator
            .to_f64()
            .and_then(|x| self.denominator.to_f64().map(|y| x / y))
    }
}

impl fmt::Binary for Rational {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}/{}", self.numerator, self.denominator)
    }
}

impl fmt::Display for Rational {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}/{}", self.numerator, self.denominator)
    }
}

impl fmt::LowerExp for Rational {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}/{}", self.numerator, self.denominator)
    }
}

impl fmt::LowerHex for Rational {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}/{}", self.numerator, self.denominator)
    }
}

impl fmt::Octal for Rational {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}/{}", self.numerator, self.denominator)
    }
}

impl fmt::Pointer for Rational {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt_ptr(self, f)
    }
}

impl fmt::UpperExp for Rational {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}/{}", self.numerator, self.denominator)
    }
}

impl fmt::UpperHex for Rational {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}/{}", self.numerator, self.denominator)
    }
}

impl ops::Add for Rational {
    type Output = Number;
    #[inline]
    fn add(self, other: Self) -> Self::Output {
        ((self.numerator * other.denominator.clone())
            + (self.denominator.clone() * other.numerator))
            / (self.denominator * other.denominator)
    }
}

impl ops::BitAnd for Rational {
    type Output = Number;
    #[inline]
    fn bitand(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::BitOr for Rational {
    type Output = Number;
    #[inline]
    fn bitor(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::BitXor for Rational {
    type Output = Number;
    #[inline]
    fn bitxor(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Div for Rational {
    type Output = Number;
    #[inline]
    fn div(self, other: Self) -> Self::Output {
        (self.numerator * other.denominator) / (self.denominator * other.numerator)
    }
}

impl ops::Mul for Rational {
    type Output = Number;
    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        (self.numerator * other.numerator) / (self.denominator * other.denominator)
    }
}

impl ops::Neg for Rational {
    type Output = Number;
    #[inline]
    fn neg(self) -> Self::Output {
        Negative::Rational(self).into()
    }
}

impl ops::Not for Rational {
    type Output = Number;
    #[inline]
    fn not(self) -> Self::Output {
        Number::NaN
    }
}

impl num_traits::pow::Pow<Rational> for Rational {
    type Output = Number;
    #[inline]
    fn pow(self, _other: Self) -> Number {
        Number::NaN
    }
}

impl ops::Rem for Rational {
    type Output = Number;
    #[inline]
    fn rem(self, other: Self) -> Self::Output {
        rem(self, other)
    }
}

impl ops::Shl for Rational {
    type Output = Number;
    #[inline]
    fn shl(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Shr for Rational {
    type Output = Number;
    #[inline]
    fn shr(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Sub for Rational {
    type Output = Number;
    #[inline]
    fn sub(self, other: Self) -> Self::Output {
        ((self.numerator * other.denominator.clone())
            - (self.denominator.clone() * other.numerator))
            / (self.denominator * other.denominator)
    }
}

#[allow(clippy::missing_trait_methods)]
impl NumberBase for Rational {
    #[inline]
    #[must_use]
    fn div_floor(self, other: Self) -> Number {
        self.numerator
            .div_floor(other.denominator)
            .div_floor(self.denominator.div_floor(other.numerator))
    }
}
