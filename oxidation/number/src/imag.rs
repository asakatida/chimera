use core::{cmp, fmt, ops};

use crate::base::Base;
use crate::natural::Natural;
use crate::negative::Negative;
use crate::number::Number;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::{fmt_ptr, gcd};

#[derive(Clone, Debug, Eq, PartialEq)]
#[non_exhaustive]
pub enum Imag {
    Base(Base),
    Natural(Natural),
    Rational(Rational),
    Negative(Negative),
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Imag {
    #[inline]
    fn eq(&self, _other: &u64) -> bool {
        false
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialOrd<u64> for Imag {
    #[inline]
    fn partial_cmp(&self, _other: &u64) -> Option<cmp::Ordering> {
        None
    }
}
#[allow(clippy::missing_trait_methods)]
impl PartialOrd<Imag> for Imag {
    #[inline]
    fn partial_cmp(&self, other: &Imag) -> Option<cmp::Ordering> {
        Some(self.cmp(other))
    }
}

#[allow(clippy::missing_trait_methods)]
impl Ord for Imag {
    #[inline]
    fn cmp(&self, other: &Imag) -> cmp::Ordering {
        match (self.clone(), other.clone()) {
            (Self::Base(a), Self::Base(b)) => a.cmp(&b),
            (Self::Natural(a), Self::Natural(b)) => a.cmp(&b),
            (Self::Negative(a), Self::Negative(b)) => a.cmp(&b),
            (Self::Rational(a), Self::Rational(b)) => a.cmp(&b),
            (_, Self::Rational(_)) => cmp::Ordering::Greater,
            (Self::Rational(_), _) => cmp::Ordering::Less,
            (_, Self::Negative(_)) => cmp::Ordering::Greater,
            (Self::Negative(_), _) => cmp::Ordering::Less,
            (_, Self::Natural(_)) => cmp::Ordering::Greater,
            (Self::Natural(_), _) => cmp::Ordering::Less,
        }
    }
}

impl fmt::Binary for Imag {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "{a}"),
            Self::Natural(a) => write!(f, "{a}"),
            Self::Rational(a) => write!(f, "{a}"),
            Self::Negative(a) => write!(f, "{a}"),
        }
    }
}

impl fmt::Display for Imag {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "{a}"),
            Self::Natural(a) => write!(f, "{a}"),
            Self::Rational(a) => write!(f, "{a}"),
            Self::Negative(a) => write!(f, "{a}"),
        }
    }
}

impl fmt::LowerExp for Imag {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "{a}"),
            Self::Natural(a) => write!(f, "{a}"),
            Self::Rational(a) => write!(f, "{a}"),
            Self::Negative(a) => write!(f, "{a}"),
        }
    }
}

impl fmt::LowerHex for Imag {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "{a}"),
            Self::Natural(a) => write!(f, "{a}"),
            Self::Rational(a) => write!(f, "{a}"),
            Self::Negative(a) => write!(f, "{a}"),
        }
    }
}

impl fmt::Octal for Imag {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "{a}"),
            Self::Natural(a) => write!(f, "{a}"),
            Self::Rational(a) => write!(f, "{a}"),
            Self::Negative(a) => write!(f, "{a}"),
        }
    }
}

impl fmt::Pointer for Imag {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt_ptr(self, f)
    }
}

impl fmt::UpperExp for Imag {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "{a}"),
            Self::Natural(a) => write!(f, "{a}"),
            Self::Rational(a) => write!(f, "{a}"),
            Self::Negative(a) => write!(f, "{a}"),
        }
    }
}

impl fmt::UpperHex for Imag {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.clone() {
            Self::Base(a) => write!(f, "{a}"),
            Self::Natural(a) => write!(f, "{a}"),
            Self::Rational(a) => write!(f, "{a}"),
            Self::Negative(a) => write!(f, "{a}"),
        }
    }
}

impl ops::Add for Imag {
    type Output = Number;

    #[inline]
    fn add(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::BitAnd for Imag {
    type Output = Number;

    #[inline]
    fn bitand(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::BitOr for Imag {
    type Output = Number;

    #[inline]
    fn bitor(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::BitXor for Imag {
    type Output = Number;

    #[inline]
    fn bitxor(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Div for Imag {
    type Output = Number;

    #[inline]
    fn div(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Mul for Imag {
    type Output = Number;

    #[inline]
    fn mul(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Neg for Imag {
    type Output = Number;

    #[inline]
    fn neg(self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Not for Imag {
    type Output = Number;

    #[inline]
    fn not(self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Rem for Imag {
    type Output = Number;

    #[inline]
    fn rem(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Shl for Imag {
    type Output = Number;

    #[inline]
    fn shl(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Shr for Imag {
    type Output = Number;

    #[inline]
    fn shr(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Sub for Imag {
    type Output = Number;

    #[inline]
    fn sub(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl NumberBase for Imag {
    #[inline]
    fn abs(&self) -> Number {
        match self.clone() {
            Self::Base(i) => i.abs(),
            Self::Natural(i) => i.abs(),
            Self::Rational(i) => i.abs(),
            Self::Negative(i) => i.abs(),
        }
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
