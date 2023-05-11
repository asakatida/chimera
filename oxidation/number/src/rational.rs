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
            numerator: match i.reduce() {
                Maybe::Base(v) => Part::Base(v),
                Maybe::Natural(v) => Part::Natural(v),
            },
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
}

#[allow(clippy::integer_division)]
#[allow(clippy::missing_trait_methods)]
impl num_traits::ToPrimitive for Rational {
    #[inline]
    fn to_i64(&self) -> Option<i64> {
        match (self.numerator.clone(), self.denominator.clone()) {
            (Part::Base(a), Part::Base(b)) => a.to_i64().and_then(|x| b.to_i64().map(|y| x / y)),
            (Part::Base(a), Part::Natural(b)) => a.to_i64().and_then(|x| b.to_i64().map(|y| x / y)),
            (Part::Natural(a), Part::Base(b)) => a.to_i64().and_then(|x| b.to_i64().map(|y| x / y)),
            (Part::Natural(a), Part::Natural(b)) => {
                a.to_i64().and_then(|x| b.to_i64().map(|y| x / y))
            }
        }
    }
    #[inline]
    fn to_u64(&self) -> Option<u64> {
        match (self.numerator.clone(), self.denominator.clone()) {
            (Part::Base(a), Part::Base(b)) => a.to_u64().and_then(|x| b.to_u64().map(|y| x / y)),
            (Part::Base(a), Part::Natural(b)) => a.to_u64().and_then(|x| b.to_u64().map(|y| x / y)),
            (Part::Natural(a), Part::Base(b)) => a.to_u64().and_then(|x| b.to_u64().map(|y| x / y)),
            (Part::Natural(a), Part::Natural(b)) => {
                a.to_u64().and_then(|x| b.to_u64().map(|y| x / y))
            }
        }
    }
    #[inline]
    fn to_f64(&self) -> Option<f64> {
        match (self.numerator.clone(), self.denominator.clone()) {
            (Part::Base(a), Part::Base(b)) => a.to_f64().and_then(|x| b.to_f64().map(|y| x / y)),
            (Part::Base(a), Part::Natural(b)) => a.to_f64().and_then(|x| b.to_f64().map(|y| x / y)),
            (Part::Natural(a), Part::Base(b)) => a.to_f64().and_then(|x| b.to_f64().map(|y| x / y)),
            (Part::Natural(a), Part::Natural(b)) => {
                a.to_f64().and_then(|x| b.to_f64().map(|y| x / y))
            }
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
        let mut numerator = match (self.numerator, other.denominator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        };
        let mut denominator = match (self.denominator, other.numerator) {
            (Part::Base(a), Part::Base(b)) => a * b,
            (Part::Base(a), Part::Natural(b)) | (Part::Natural(b), Part::Base(a)) => b * a.into(),
            (Part::Natural(a), Part::Natural(b)) => a * b,
        };
        let gcd = numerator.clone().gcd(denominator.clone());
        if gcd != 1 {
            numerator = numerator.div_floor(gcd.clone());
            denominator = denominator.div_floor(gcd);
        }
        match (numerator, denominator) {
            (Number::Base(a), Number::Base(b)) => Some(Rational {
                numerator: Part::Base(a),
                denominator: Part::Base(b),
            }),
            (Number::Base(a), Number::Natural(b)) => Some(Rational {
                numerator: Part::Base(a),
                denominator: Part::Natural(b),
            }),
            (Number::Natural(a), Number::Base(b)) => Some(Rational {
                numerator: Part::Natural(a),
                denominator: Part::Base(b),
            }),
            (Number::Natural(a), Number::Natural(b)) => Some(Rational {
                numerator: Part::Natural(a),
                denominator: Part::Natural(b),
            }),
            _ => None,
        }
        .map_or(Number::NaN, Into::into)
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

#[allow(clippy::missing_trait_methods)]
impl NumberBase for Rational {
    #[inline]
    fn abs(self) -> Number {
        self.into()
    }
    #[inline]
    fn div_floor(self, other: Self) -> Number {
        (match (self.numerator, other.denominator) {
            (Part::Base(a), Part::Base(b)) => a.div_floor(b),
            (Part::Base(a), Part::Natural(b)) => Natural::from(a).div_floor(b),
            (Part::Natural(a), Part::Base(b)) => a.div_floor(b.into()),
            (Part::Natural(a), Part::Natural(b)) => a.div_floor(b),
        })
        .div_floor(match (self.denominator, other.numerator) {
            (Part::Base(a), Part::Base(b)) => a.div_floor(b),
            (Part::Base(a), Part::Natural(b)) => Natural::from(a).div_floor(b),
            (Part::Natural(a), Part::Base(b)) => a.div_floor(b.into()),
            (Part::Natural(a), Part::Natural(b)) => a.div_floor(b),
        })
    }
}
