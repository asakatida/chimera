use core::{cmp, fmt, ops};

use crate::base::Base;
use crate::imag::Imag;
use crate::negative::Negative;
use crate::number::Number;
use crate::rational::Rational;
use crate::traits::NumberBase;
use crate::utils::{condense_bigint, fmt_ptr, gcd};

#[derive(Clone, Debug, Eq, PartialEq)]
#[non_exhaustive]
pub struct Complex {
    pub real: Imag,
    pub imag: Imag,
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
    fn partial_cmp(&self, other: &Complex) -> Option<cmp::Ordering> {
        Some(self.cmp(other))
    }
}

#[allow(clippy::missing_trait_methods)]
impl Ord for Complex {
    #[inline]
    fn cmp(&self, _other: &Complex) -> cmp::Ordering {
        cmp::Ordering::Equal
    }
}

impl From<u64> for Complex {
    #[inline]
    fn from(i: u64) -> Self {
        Complex {
            real: Imag::Base(i.into()),
            imag: Imag::Base(0.into()),
        }
    }
}
impl From<Base> for Complex {
    #[inline]
    fn from(i: Base) -> Self {
        Complex {
            real: Imag::Base(i),
            imag: Imag::Base(0.into()),
        }
    }
}
impl From<&Base> for Complex {
    #[inline]
    fn from(i: &Base) -> Self {
        Complex {
            real: Imag::Base(*i),
            imag: Imag::Base(0.into()),
        }
    }
}
impl From<num_bigint::BigUint> for Complex {
    #[inline]
    fn from(i: num_bigint::BigUint) -> Self {
        Complex {
            real: condense_bigint(i).map_or_else(Imag::Base, Imag::Natural),
            imag: Imag::Base(0.into()),
        }
    }
}
impl From<&num_bigint::BigUint> for Complex {
    #[inline]
    fn from(i: &num_bigint::BigUint) -> Self {
        i.clone().into()
    }
}
impl From<Rational> for Complex {
    #[inline]
    fn from(i: Rational) -> Self {
        Complex {
            real: Imag::Rational(i),
            imag: Imag::Base(0.into()),
        }
    }
}
impl From<&Rational> for Complex {
    #[inline]
    fn from(i: &Rational) -> Self {
        i.clone().into()
    }
}
impl From<Negative> for Complex {
    #[inline]
    fn from(i: Negative) -> Self {
        Complex {
            real: Imag::Negative(i),
            imag: Imag::Base(0.into()),
        }
    }
}
impl From<&Negative> for Complex {
    #[inline]
    fn from(i: &Negative) -> Self {
        i.clone().into()
    }
}
impl From<Imag> for Complex {
    #[inline]
    fn from(i: Imag) -> Self {
        Complex {
            real: Imag::Base(0.into()),
            imag: i,
        }
    }
}
impl From<&Imag> for Complex {
    #[inline]
    fn from(i: &Imag) -> Self {
        i.clone().into()
    }
}

impl Complex {
    #[inline]
    #[must_use]
    pub fn new(a: Imag, b: Imag) -> Self {
        Complex { real: a, imag: b }
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

impl fmt::Binary for Complex {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} + {}i", self.real, self.imag)
    }
}

impl fmt::Display for Complex {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} + {}i", self.real, self.imag)
    }
}

impl fmt::LowerExp for Complex {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.real)
    }
}

impl fmt::LowerHex for Complex {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:x} + {:x}i", self.real, self.imag)
    }
}

impl fmt::Octal for Complex {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:o} + {:o}i", self.real, self.imag)
    }
}

impl fmt::Pointer for Complex {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt_ptr(self, f)
    }
}

impl fmt::UpperExp for Complex {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.real)
    }
}

impl fmt::UpperHex for Complex {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:X} + {:X}i", self.real, self.imag)
    }
}

impl ops::Add for Complex {
    type Output = Number;

    #[inline]
    fn add(self, other: Self) -> Self::Output {
        self.real + other.real + (self.imag + other.imag).imag()
    }
}

impl ops::BitAnd for Complex {
    type Output = Number;

    #[inline]
    fn bitand(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::BitOr for Complex {
    type Output = Number;

    #[inline]
    fn bitor(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::BitXor for Complex {
    type Output = Number;

    #[inline]
    fn bitxor(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Div for Complex {
    type Output = Number;

    #[inline]
    fn div(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Mul for Complex {
    type Output = Number;

    #[inline]
    fn mul(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Neg for Complex {
    type Output = Number;

    #[inline]
    fn neg(self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Not for Complex {
    type Output = Number;

    #[inline]
    fn not(self) -> Self::Output {
        Number::NaN
    }
}

impl num_traits::pow::Pow<Complex> for Complex {
    type Output = Number;

    #[inline]
    fn pow(self, _other: Self) -> Number {
        Number::NaN
    }
}

impl ops::Rem for Complex {
    type Output = Number;

    #[inline]
    fn rem(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Shl for Complex {
    type Output = Number;

    #[inline]
    fn shl(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Shr for Complex {
    type Output = Number;

    #[inline]
    fn shr(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl ops::Sub for Complex {
    type Output = Number;

    #[inline]
    fn sub(self, _other: Self) -> Self::Output {
        Number::NaN
    }
}

impl NumberBase for Complex {
    #[inline]
    fn abs(self) -> Number {
        self.real.abs() + self.imag.abs()
    }

    #[inline]
    fn div_floor(self, other: Self) -> Number {
        self.real.div_floor(other.real) + self.imag.div_floor(other.imag)
    }

    #[inline]
    fn gcd(self, other: Self) -> Number {
        gcd(self, other)
    }
}
