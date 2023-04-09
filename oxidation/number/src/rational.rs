use core::{cmp, fmt, ops};

use crate::base::Base;
use crate::natural::Natural;
use crate::negative::Negative;
use crate::number::Number;
use crate::traits::NumberBase;
use crate::utils::{fmt_ptr, gcd, rem};

#[non_exhaustive]
#[derive(Clone, Debug, Eq, PartialEq)]
pub enum Part {
    Base(Base),
    Natural(Natural),
}

impl fmt::Display for Part {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self.clone() {
            Part::Base(n) => write!(f, "{n}"),
            Part::Natural(n) => write!(f, "{n}"),
        }
    }
}

#[non_exhaustive]
#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Rational {
    pub numerator: Part,
    pub denominator: Part,
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
            match self.numerator.clone() {
                Part::Base(a) => Number::from(a),
                Part::Natural(a) => Number::from(a),
            }
            .cmp(&match self.denominator.clone() {
                Part::Base(a) => a * (*other).into(),
                Part::Natural(a) => a * (*other).into(),
            }),
        )
    }
}
#[allow(clippy::missing_trait_methods)]
impl PartialOrd<Rational> for Rational {
    #[inline]
    fn partial_cmp(&self, other: &Rational) -> Option<cmp::Ordering> {
        Some(self.cmp(other))
    }
}

#[allow(clippy::missing_trait_methods)]
impl Ord for Rational {
    #[inline]
    fn cmp(&self, other: &Rational) -> cmp::Ordering {
        match (self.numerator.clone(), other.denominator.clone()) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Natural(a), Part::Base(b)) | (Part::Base(b), Part::Natural(a)) => a * b.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }
        .cmp(&match (other.numerator.clone(), self.denominator.clone()) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Natural(a), Part::Base(b)) | (Part::Base(b), Part::Natural(a)) => a * b.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        })
    }
}

impl From<u64> for Rational {
    #[inline]
    fn from(i: u64) -> Self {
        Rational {
            numerator: Part::Base(i.into()),
            denominator: Part::Base(1.into()),
        }
    }
}
impl From<Base> for Rational {
    #[inline]
    fn from(i: Base) -> Self {
        Rational {
            numerator: Part::Base(i),
            denominator: Part::Base(1.into()),
        }
    }
}
impl From<&Base> for Rational {
    #[inline]
    fn from(i: &Base) -> Self {
        Rational {
            numerator: Part::Base(*i),
            denominator: Part::Base(1.into()),
        }
    }
}
impl From<Natural> for Rational {
    #[inline]
    fn from(i: Natural) -> Self {
        Rational {
            numerator: Part::Natural(i),
            denominator: Part::Base(1.into()),
        }
    }
}
impl From<&Natural> for Rational {
    #[inline]
    fn from(i: &Natural) -> Self {
        i.clone().into()
    }
}

impl Rational {
    #[inline]
    #[must_use]
    pub fn new(a: Part, b: Part) -> Self {
        Rational {
            numerator: a,
            denominator: b,
        }
    }

    #[inline]
    #[must_use]
    pub fn as_f64(self) -> f64 {
        match (self.numerator.clone(), self.denominator) {
            (Part::Base(a), Part::Base(b)) => a.as_f64() / b.as_f64(),
            (Part::Base(a), Part::Natural(b)) => a.as_f64() / b.as_f64(),
            (Part::Natural(a), Part::Base(b)) => a.as_f64() / b.as_f64(),
            (Part::Natural(a), Part::Natural(b)) => a.as_f64() / b.as_f64(),
        }
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
        (match (self.numerator, other.denominator.clone()) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        } + match (self.denominator.clone(), other.numerator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }) / match (self.denominator, other.denominator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }
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
        (match (self.numerator, other.denominator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }) / match (self.denominator, other.numerator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }
    }
}

impl ops::Mul for Rational {
    type Output = Number;

    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        (match (self.numerator, other.numerator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }) / match (self.denominator, other.denominator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }
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
        (match (self.numerator, other.denominator.clone()) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        } - match (self.denominator.clone(), other.numerator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }) / match (self.denominator, other.denominator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        }
    }
}

impl NumberBase for Rational {
    #[inline]
    fn abs(&self) -> Number {
        self.clone().into()
    }

    #[inline]
    fn gcd(&self, other: &Self) -> Number {
        gcd(self, other)
    }

    #[inline]
    fn pow(&self, _other: &Self) -> Number {
        Number::NaN
    }
}
