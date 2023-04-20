use core::{cmp, fmt, ops};

use num_integer::Integer;

use crate::base::Base;
use crate::number::Number;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::{fmt_ptr, gcd};

#[non_exhaustive]
#[derive(Clone, Debug, Eq, PartialEq)]
pub enum Negative {
    Base(Base),
    Natural(num_bigint::BigUint),
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
    fn partial_cmp(&self, other: &Negative) -> Option<cmp::Ordering> {
        Some(self.cmp(other))
    }
}

#[allow(clippy::missing_trait_methods)]
impl Ord for Negative {
    #[inline]
    fn cmp(&self, other: &Negative) -> cmp::Ordering {
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

impl fmt::Binary for Negative {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "-{a}"),
            Self::Natural(a) => write!(f, "-{a}"),
            Self::Rational(a) => write!(f, "-{a}"),
        }
    }
}

impl fmt::Display for Negative {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "-{a}"),
            Self::Natural(a) => write!(f, "-{a}"),
            Self::Rational(a) => write!(f, "-{a}"),
        }
    }
}

impl fmt::LowerExp for Negative {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "-{a}"),
            Self::Natural(a) => write!(f, "-{a}"),
            Self::Rational(a) => write!(f, "-{a}"),
        }
    }
}

impl fmt::LowerHex for Negative {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "-{a}"),
            Self::Natural(a) => write!(f, "-{a}"),
            Self::Rational(a) => write!(f, "-{a}"),
        }
    }
}

impl fmt::Octal for Negative {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "-{a}"),
            Self::Natural(a) => write!(f, "-{a}"),
            Self::Rational(a) => write!(f, "-{a}"),
        }
    }
}

impl fmt::Pointer for Negative {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt_ptr(self, f)
    }
}

impl fmt::UpperExp for Negative {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "-{a}"),
            Self::Natural(a) => write!(f, "-{a}"),
            Self::Rational(a) => write!(f, "-{a}"),
        }
    }
}

impl fmt::UpperHex for Negative {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "-{a}"),
            Self::Natural(a) => write!(f, "-{a}"),
            Self::Rational(a) => write!(f, "-{a}"),
        }
    }
}

impl ops::Add for Negative {
    type Output = Number;
    #[inline]
    fn add(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => -(a + b),
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => {
                Self::Natural(b + Into::<num_bigint::BigUint>::into(a)).into()
            }
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => {
                -(b + a.into())
            }
            (Self::Natural(a), Self::Natural(b)) => Self::Natural(a + b).into(),
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                -(b + a.into())
            }
            (Self::Rational(a), Self::Rational(b)) => -(a + b),
        }
    }
}

impl ops::BitAnd for Negative {
    type Output = Number;
    #[inline]
    fn bitand(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => -(a & b),
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => {
                Self::Natural(b & Into::<num_bigint::BigUint>::into(a)).into()
            }
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => {
                -(b & a.into())
            }
            (Self::Natural(a), Self::Natural(b)) => Self::Natural(a & b).into(),
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                -(b & a.into())
            }
            (Self::Rational(a), Self::Rational(b)) => -(a & b),
        }
    }
}

impl ops::BitOr for Negative {
    type Output = Number;
    #[inline]
    fn bitor(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => -(a | b),
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => {
                Self::Natural(b & Into::<num_bigint::BigUint>::into(a)).into()
            }
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => {
                -(b | a.into())
            }
            (Self::Natural(a), Self::Natural(b)) => Self::Natural(a | b).into(),
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                -(b | a.into())
            }
            (Self::Rational(a), Self::Rational(b)) => -(a | b),
        }
    }
}

impl ops::BitXor for Negative {
    type Output = Number;
    #[inline]
    fn bitxor(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a ^ b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => {
                Self::Natural(b ^ Into::<num_bigint::BigUint>::into(a)).into()
            }
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b ^ a.into(),
            (Self::Natural(a), Self::Natural(b)) => Self::Natural(a ^ b).into(),
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b ^ a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a ^ b,
        }
    }
}

impl ops::Div for Negative {
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

impl ops::Mul for Negative {
    type Output = Number;
    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a * b,
            (Self::Base(a), Self::Natural(b)) | (Self::Natural(b), Self::Base(a)) => {
                (b * Into::<num_bigint::BigUint>::into(a)).into()
            }
            (Self::Base(a), Self::Rational(b)) | (Self::Rational(b), Self::Base(a)) => b * a.into(),
            (Self::Natural(a), Self::Natural(b)) => (a * b).into(),
            (Self::Natural(a), Self::Rational(b)) | (Self::Rational(b), Self::Natural(a)) => {
                b * a.into()
            }
            (Self::Rational(a), Self::Rational(b)) => a * b,
        }
    }
}

impl ops::Neg for Negative {
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

impl ops::Not for Negative {
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

impl num_traits::pow::Pow<Negative> for Negative {
    type Output = Number;
    #[inline]
    fn pow(self, _other: Self) -> Number {
        Number::NaN
    }
}

impl ops::Rem for Negative {
    type Output = Number;
    #[inline]
    fn rem(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a % b,
            (Self::Base(a), Self::Natural(b)) => (Into::<num_bigint::BigUint>::into(a) % b).into(),
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) % b,
            (Self::Natural(a), Self::Base(b)) => (a % Into::<num_bigint::BigUint>::into(b)).into(),
            (Self::Natural(a), Self::Natural(b)) => (a % b).into(),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) % b,
            (Self::Rational(a), Self::Base(b)) => a % b.into(),
            (Self::Rational(a), Self::Natural(b)) => a % b.into(),
            (Self::Rational(a), Self::Rational(b)) => a % b,
        }
    }
}

impl ops::Shl for Negative {
    type Output = Number;
    #[inline]
    fn shl(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a << b,
            (_, Self::Natural(_)) => Number::NaN,
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) << b,
            (Self::Natural(a), Self::Base(b)) => (a << b.value).into(),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) << b,
            (Self::Rational(a), Self::Base(b)) => a << b.into(),
            (Self::Rational(a), Self::Rational(b)) => a << b,
        }
    }
}

impl ops::Shr for Negative {
    type Output = Number;
    #[inline]
    fn shr(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a >> b,
            (_, Self::Natural(_)) => Number::NaN,
            (Self::Base(a), Self::Rational(b)) => Rational::from(a) >> b,
            (Self::Natural(a), Self::Base(b)) => (a >> b.value).into(),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a) >> b,
            (Self::Rational(a), Self::Base(b)) => a >> b.into(),
            (Self::Rational(a), Self::Rational(b)) => a >> b,
        }
    }
}

impl ops::Sub for Negative {
    type Output = Number;
    #[inline]
    fn sub(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => -(b - a),
            (Self::Base(a), Self::Natural(b)) => {
                Self::Natural(b - Into::<num_bigint::BigUint>::into(a)).into()
            }
            (Self::Base(a), Self::Rational(b)) => -(b - a.into()),
            (Self::Natural(a), Self::Base(b)) => {
                Self::Natural(Into::<num_bigint::BigUint>::into(b) - a).into()
            }
            (Self::Natural(a), Self::Natural(b)) => Self::Natural(b - a).into(),
            (Self::Natural(a), Self::Rational(b)) => -(b - a.into()),
            (Self::Rational(a), Self::Base(b)) => -(Rational::from(b) - a),
            (Self::Rational(a), Self::Natural(b)) => -(Rational::from(b) - a),
            (Self::Rational(a), Self::Rational(b)) => -(b - a),
        }
    }
}

impl NumberBase for Negative {
    #[inline]
    fn abs(self) -> Number {
        match self {
            Self::Base(a) => a.into(),
            Self::Natural(a) => a.into(),
            Self::Rational(a) => a.into(),
        }
    }
    #[inline]
    fn div_floor(self, other: Self) -> Number {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a.div_floor(b),
            (Self::Base(a), Self::Natural(b)) => {
                Into::<num_bigint::BigUint>::into(a).div_floor(&b).into()
            }
            (Self::Base(a), Self::Rational(b)) => Rational::from(a).div_floor(b),
            (Self::Natural(a), Self::Base(b)) => {
                a.div_floor(&Into::<num_bigint::BigUint>::into(b)).into()
            }
            (Self::Natural(a), Self::Natural(b)) => a.div_floor(&b).into(),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a).div_floor(b),
            (Self::Rational(a), Self::Base(b)) => a.div_floor(b.into()),
            (Self::Rational(a), Self::Natural(b)) => a.div_floor(b.into()),
            (Self::Rational(a), Self::Rational(b)) => a.div_floor(b),
        }
    }
    #[inline]
    fn gcd(self, other: Self) -> Number {
        gcd(self, other)
    }
}
