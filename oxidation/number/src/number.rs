use core::{cmp, fmt, ops};

use crate::base::Base;
use crate::complex::Complex;
use crate::imag::Imag;
use crate::natural::Natural;
use crate::negative::Negative;
use crate::rational::{Part, Rational};
use crate::traits::NumberBase;
use crate::utils::fmt_ptr;

#[non_exhaustive]
#[derive(Clone, Debug, Eq, PartialEq)]
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
    fn partial_cmp(&self, other: &Number) -> Option<cmp::Ordering> {
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
    fn cmp(&self, other: &Number) -> cmp::Ordering {
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
impl From<&Base> for Number {
    #[inline]
    fn from(i: &Base) -> Self {
        Self::Base(*i)
    }
}
impl From<Natural> for Number {
    #[inline]
    fn from(i: Natural) -> Self {
        let mut value = i.value;
        while value.ends_with(&[0]) {
            value.pop();
        }
        if value.len() < 2 {
            value.pop().unwrap_or_default().into()
        } else {
            Self::Natural(Natural { value })
        }
    }
}
impl From<&Natural> for Number {
    #[inline]
    fn from(i: &Natural) -> Self {
        i.clone().into()
    }
}
impl From<Rational> for Number {
    #[inline]
    fn from(i: Rational) -> Self {
        if match i.denominator.clone() {
            Part::Base(v) => v == 0,
            Part::Natural(v) => v == 0,
        } {
            Self::Rational(i)
        } else {
            let gcd = match (i.numerator.clone(), i.denominator.clone()) {
                (Part::Base(a), Part::Base(b)) => a.gcd(&b),
                (Part::Natural(a), Part::Base(b)) => a.gcd(&b.into()),
                (Part::Base(a), Part::Natural(b)) => b.gcd(&a.into()),
                (Part::Natural(a), Part::Natural(b)) => a.gcd(&b),
            };
            if gcd == 1 {
                Self::Rational(i)
            } else {
                match (i.numerator, i.denominator) {
                    (Part::Base(a), Part::Base(b)) => {
                        (Self::from(a) / gcd.clone()) / (Self::from(b) / gcd)
                    }
                    (Part::Natural(a), Part::Base(b)) => {
                        (Self::from(a) / gcd.clone()) / (Self::from(b) / gcd)
                    }
                    (Part::Base(a), Part::Natural(b)) => {
                        (Self::from(a) / gcd.clone()) / (Self::from(b) / gcd)
                    }
                    (Part::Natural(a), Part::Natural(b)) => {
                        (Self::from(a) / gcd.clone()) / (Self::from(b) / gcd)
                    }
                }
            }
        }
    }
}
impl From<&Rational> for Number {
    #[inline]
    fn from(i: &Rational) -> Self {
        i.clone().into()
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
impl From<&Negative> for Number {
    #[inline]
    fn from(i: &Negative) -> Self {
        i.clone().into()
    }
}
impl From<Imag> for Number {
    #[inline]
    fn from(i: Imag) -> Self {
        if i == 0 {
            0.into()
        } else {
            Self::Imag(i)
        }
    }
}
impl From<&Imag> for Number {
    #[inline]
    fn from(i: &Imag) -> Self {
        i.clone().into()
    }
}
impl From<Complex> for Number {
    #[inline]
    fn from(i: Complex) -> Self {
        if i.imag == 0 {
            match i.real {
                Imag::Base(a) => a.into(),
                Imag::Natural(a) => a.into(),
                Imag::Rational(a) => a.into(),
                Imag::Negative(a) => a.into(),
            }
        } else if i.real == 0 {
            i.imag.into()
        } else {
            Self::Complex(i)
        }
    }
}
impl From<&Complex> for Number {
    #[inline]
    fn from(i: &Complex) -> Self {
        i.clone().into()
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
    pub fn imag(&self) -> Self {
        match self.clone() {
            Self::NaN => Self::NaN,
            Self::Base(a) => Imag::Base(a).into(),
            Self::Natural(a) => Imag::Natural(a).into(),
            Self::Rational(a) => Imag::Rational(a).into(),
            Self::Negative(a) => Imag::Negative(a).into(),
            Self::Imag(a) => a.into(),
            Self::Complex(i) => Complex {
                real: i.imag,
                imag: i.real,
            }
            .into(),
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
            Self::Natural(a) => a.fmt(f),
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
            Self::Natural(a) => fmt::LowerExp::fmt(&a, f),
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
            Self::Natural(a) => fmt::UpperExp::fmt(&a, f),
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
    type Output = Number;

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
    type Output = Number;

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
    type Output = Number;

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
            (n, Self::Negative(i)) | (Self::Negative(i), n) => match (i, n) {
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
    type Output = Number;

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
    type Output = Number;

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
                (Negative::Base(b), Self::Base(a)) => a / b,
                (Negative::Base(b), Self::Natural(a)) => a / b.into(),
                (Negative::Base(b), Self::Rational(a)) => a / b.into(),
                (Negative::Base(b), Self::Imag(a)) => Complex::from(a) / b.into(),
                (Negative::Base(b), Self::Complex(a)) => a / b.into(),
                (Negative::Natural(b), Self::Base(a)) => Natural::from(a) / b,
                (Negative::Natural(b), Self::Natural(a)) => a / b,
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
                (Negative::Base(a), Self::Base(b)) => a / b,
                (Negative::Base(a), Self::Natural(b)) => Natural::from(a) / b,
                (Negative::Base(a), Self::Rational(b)) => Rational::from(a) / b,
                (Negative::Base(a), Self::Imag(b)) => Complex::from(a) / b.into(),
                (Negative::Base(a), Self::Complex(b)) => Complex::from(a) / b,
                (Negative::Natural(a), Self::Base(b)) => a / b.into(),
                (Negative::Natural(a), Self::Natural(b)) => a / b,
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
    type Output = Number;

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
    type Output = Number;

    #[inline]
    fn neg(self) -> Self::Output {
        match self {
            Self::Base(a) => -a,
            Self::Natural(a) => -a,
            Self::Rational(a) => -a,
            Self::Negative(a) => -a,
            Self::Imag(a) => -a,
            Self::Complex(a) => -a,
            Self::NaN => Self::NaN,
        }
    }
}

impl ops::Not for Number {
    type Output = Number;

    #[inline]
    fn not(self) -> Self::Output {
        match self {
            Self::Base(a) => !a,
            Self::Natural(a) => !a,
            Self::Rational(a) => !a,
            Self::Negative(a) => !a,
            Self::Imag(a) => !a,
            Self::Complex(a) => !a,
            Self::NaN => Self::NaN,
        }
    }
}

impl ops::Rem for Number {
    type Output = Number;

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
            (Self::Base(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -(a % b),
                Negative::Natural(b) => -(Natural::from(a) % b),
                Negative::Rational(b) => -(Rational::from(a) % b),
            },
            (Self::Natural(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -(a % b.into()),
                Negative::Natural(b) => -(a % b),
                Negative::Rational(b) => -(Rational::from(a) % b),
            },
            (Self::Rational(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -(a % b.into()),
                Negative::Natural(b) => -(a % b.into()),
                Negative::Rational(b) => -(a % b),
            },
            (Self::Negative(i), Self::Base(b)) => match i {
                Negative::Base(a) => -(a % b),
                Negative::Natural(a) => -(a % b.into()),
                Negative::Rational(a) => -(a % b.into()),
            },
            (Self::Negative(i), Self::Natural(b)) => match i {
                Negative::Base(a) => -(Natural::from(a) % b),
                Negative::Natural(a) => -(a % b),
                Negative::Rational(a) => -(a % b.into()),
            },
            (Self::Negative(i), Self::Rational(b)) => match i {
                Negative::Base(a) => -(Rational::from(a) % b),
                Negative::Natural(a) => -(Rational::from(a) % b),
                Negative::Rational(a) => -(a % b),
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
    type Output = Number;

    #[inline]
    fn shl(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a << b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) << b,
            (Self::Base(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a >> b,
                Negative::Natural(b) => Natural::from(a) >> b,
                Negative::Rational(_) => Self::NaN,
            },
            (Self::Natural(a), Self::Base(b)) => a << b.into(),
            (Self::Natural(a), Self::Natural(b)) => a << b,
            (Self::Natural(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a >> b.into(),
                Negative::Natural(b) => a >> b,
                Negative::Rational(_) => Self::NaN,
            },
            (Self::Negative(i), Self::Base(b)) => match i {
                Negative::Base(a) => -(a << b),
                Negative::Natural(a) => -(a << b.into()),
                Negative::Rational(_) => Self::NaN,
            },
            (Self::Negative(i), Self::Natural(b)) => match i {
                Negative::Base(a) => -(Natural::from(a) << b),
                Negative::Natural(a) => -(a << b),
                Negative::Rational(_) => Self::NaN,
            },
            (Self::Negative(i), Self::Negative(j)) => match (i, j) {
                (Negative::Base(a), Negative::Base(b)) => -(a >> b),
                (Negative::Base(a), Negative::Natural(b)) => -(Natural::from(a) >> b),
                (Negative::Natural(a), Negative::Base(b)) => -(a >> b.into()),
                (Negative::Natural(a), Negative::Natural(b)) => -(a >> b),
                (Negative::Rational(_), _) | (_, Negative::Rational(_)) => Self::NaN,
            },
            (Self::Rational(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN, _)
            | (_, Self::Rational(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
        }
    }
}

impl ops::Shr for Number {
    type Output = Number;

    #[inline]
    fn shr(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::Base(a), Self::Base(b)) => a >> b,
            (Self::Base(a), Self::Natural(b)) => Natural::from(a) >> b,
            (Self::Base(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a << b,
                Negative::Natural(b) => Natural::from(a) << b,
                Negative::Rational(_) => Self::NaN,
            },
            (Self::Natural(a), Self::Base(b)) => a >> b.into(),
            (Self::Natural(a), Self::Natural(b)) => a >> b,
            (Self::Natural(a), Self::Negative(i)) => match i {
                Negative::Base(b) => a << b.into(),
                Negative::Natural(b) => a << b,
                Negative::Rational(_) => Self::NaN,
            },
            (Self::Negative(i), Self::Base(b)) => match i {
                Negative::Base(a) => -(a >> b),
                Negative::Natural(a) => -(a >> b.into()),
                Negative::Rational(_) => Self::NaN,
            },
            (Self::Negative(i), Self::Natural(b)) => match i {
                Negative::Base(a) => -(Natural::from(a) >> b),
                Negative::Natural(a) => -(a >> b),
                Negative::Rational(_) => Self::NaN,
            },
            (Self::Negative(i), Self::Negative(j)) => match (i, j) {
                (Negative::Base(a), Negative::Base(b)) => -(a << b),
                (Negative::Base(a), Negative::Natural(b)) => -(Natural::from(a) << b),
                (Negative::Natural(a), Negative::Base(b)) => -(a << b.into()),
                (Negative::Natural(a), Negative::Natural(b)) => -(a << b),
                (Negative::Rational(_), _) | (_, Negative::Rational(_)) => Self::NaN,
            },
            (Self::Rational(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN, _)
            | (_, Self::Rational(_) | Self::Imag(_) | Self::Complex(_) | Self::NaN) => Self::NaN,
        }
    }
}

impl ops::Sub for Number {
    type Output = Number;

    #[inline]
    fn sub(self, other: Self) -> Self::Output {
        match (self, other) {
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
            (Self::Base(a), Self::Base(b)) => a - b,
            (Self::Base(a), Self::Natural(b)) => -(b - a.into()),
            (Self::Natural(a), Self::Base(b)) => a - b.into(),
            (Self::Natural(a), Self::Natural(b)) => a - b,
            (Self::Base(a), Self::Rational(b)) => -(b - a.into()),
            (Self::Natural(a), Self::Rational(b)) => -(b - a.into()),
            (Self::Rational(a), Self::Base(b)) => a - b.into(),
            (Self::Rational(a), Self::Natural(b)) => a - b.into(),
            (Self::Rational(a), Self::Rational(b)) => a - b,
            (Self::Base(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -(a - b),
                Negative::Natural(b) => b - a.into(),
                Negative::Rational(b) => b - a.into(),
            },
            (Self::Natural(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -(a - b.into()),
                Negative::Natural(b) => -(a - b),
                Negative::Rational(b) => b - a.into(),
            },
            (Self::Rational(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -(a - b.into()),
                Negative::Natural(b) => -(a - b.into()),
                Negative::Rational(b) => -(a - b),
            },
            (Self::Negative(i), Self::Base(b)) => match i {
                Negative::Base(a) => b - a,
                Negative::Natural(a) => -(a - b.into()),
                Negative::Rational(a) => -(a - b.into()),
            },
            (Self::Negative(i), Self::Natural(b)) => match i {
                Negative::Base(a) => b - a.into(),
                Negative::Natural(a) => b - a,
                Negative::Rational(a) => -(a - b.into()),
            },
            (Self::Negative(i), Self::Rational(b)) => match i {
                Negative::Base(a) => b - a.into(),
                Negative::Natural(a) => b - a.into(),
                Negative::Rational(a) => b - a,
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

impl NumberBase for Number {
    #[inline]
    fn abs(&self) -> Number {
        match self.clone() {
            Self::Base(a) => a.abs(),
            Self::Natural(a) => a.abs(),
            Self::Rational(a) => a.abs(),
            Self::Negative(a) => a.abs(),
            Self::Imag(a) => a.abs(),
            Self::Complex(a) => a.abs(),
            Self::NaN => Self::NaN,
        }
    }

    #[inline]
    fn gcd(&self, other: &Self) -> Self {
        match (self.clone(), other.clone()) {
            (Self::Base(a), Self::Base(b)) => a.gcd(&b),
            (Self::Base(a), Self::Natural(b)) => Natural::from(a).gcd(&b),
            (Self::Natural(a), Self::Base(b)) => a.gcd(&b.into()),
            (Self::Natural(a), Self::Natural(b)) => a.gcd(&b),
            (Self::Base(a), Self::Rational(b)) => Rational::from(a).gcd(&b),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a).gcd(&b),
            (Self::Rational(a), Self::Base(b)) => a.gcd(&b.into()),
            (Self::Rational(a), Self::Natural(b)) => a.gcd(&b.into()),
            (Self::Rational(a), Self::Rational(b)) => a.gcd(&b),
            (Self::Base(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -a.gcd(&b),
                Negative::Natural(b) => -Natural::from(a).gcd(&b),
                Negative::Rational(b) => -Rational::from(a).gcd(&b),
            },
            (Self::Natural(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -a.gcd(&b.into()),
                Negative::Natural(b) => -a.gcd(&b),
                Negative::Rational(b) => -Rational::from(a).gcd(&b),
            },
            (Self::Rational(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -a.gcd(&b.into()),
                Negative::Natural(b) => -a.gcd(&b.into()),
                Negative::Rational(b) => -a.gcd(&b),
            },
            (Self::Negative(i), Self::Base(b)) => match i {
                Negative::Base(a) => b.gcd(&a),
                Negative::Natural(a) => Natural::from(b).gcd(&a),
                Negative::Rational(a) => Rational::from(b).gcd(&a),
            },
            (Self::Negative(i), Self::Natural(b)) => match i {
                Negative::Base(a) => b.gcd(&a.into()),
                Negative::Natural(a) => b.gcd(&a),
                Negative::Rational(a) => Rational::from(b).gcd(&a),
            },
            (Self::Negative(i), Self::Rational(b)) => match i {
                Negative::Base(a) => b.gcd(&a.into()),
                Negative::Natural(a) => b.gcd(&a.into()),
                Negative::Rational(a) => b.gcd(&a),
            },
            (Self::Negative(a), Self::Negative(b)) => a.gcd(&b),
            (Self::Base(a), Self::Imag(b)) => Complex::from(a).gcd(&b.into()),
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a).gcd(&b.into()),
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a).gcd(&b.into()),
            (Self::Negative(a), Self::Imag(b)) => Complex::from(a).gcd(&b.into()),
            (Self::Imag(a), Self::Base(b)) => Complex::from(a).gcd(&b.into()),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a).gcd(&b.into()),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a).gcd(&b.into()),
            (Self::Imag(a), Self::Negative(b)) => Complex::from(a).gcd(&b.into()),
            (Self::Imag(a), Self::Imag(b)) => a.gcd(&b),
            (Self::Base(a), Self::Complex(b)) => Complex::from(a).gcd(&b),
            (Self::Natural(a), Self::Complex(b)) => Complex::from(a).gcd(&b),
            (Self::Rational(a), Self::Complex(b)) => Complex::from(a).gcd(&b),
            (Self::Negative(a), Self::Complex(b)) => Complex::from(a).gcd(&b),
            (Self::Imag(a), Self::Complex(b)) => Complex::from(a).gcd(&b),
            (Self::Complex(a), Self::Base(b)) => a.gcd(&b.into()),
            (Self::Complex(a), Self::Natural(b)) => a.gcd(&b.into()),
            (Self::Complex(a), Self::Rational(b)) => a.gcd(&b.into()),
            (Self::Complex(a), Self::Negative(b)) => a.gcd(&b.into()),
            (Self::Complex(a), Self::Imag(b)) => a.gcd(&b.into()),
            (Self::Complex(a), Self::Complex(b)) => a.gcd(&b),
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
        }
    }

    #[inline]
    fn pow(&self, other: &Self) -> Self {
        match (self.clone(), other.clone()) {
            (Self::Base(a), Self::Base(b)) => a.pow(&b),
            (Self::Base(a), Self::Natural(b)) => Natural::from(a).pow(&b),
            (Self::Natural(a), Self::Base(b)) => a.pow(&b.into()),
            (Self::Natural(a), Self::Natural(b)) => a.pow(&b),
            (Self::Base(a), Self::Rational(b)) => Rational::from(a).pow(&b),
            (Self::Natural(a), Self::Rational(b)) => Rational::from(a).pow(&b),
            (Self::Rational(a), Self::Base(b)) => a.pow(&b.into()),
            (Self::Rational(a), Self::Natural(b)) => a.pow(&b.into()),
            (Self::Rational(a), Self::Rational(b)) => a.pow(&b),
            (Self::Base(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -a.pow(&b),
                Negative::Natural(b) => -Natural::from(a).pow(&b),
                Negative::Rational(b) => -Rational::from(a).pow(&b),
            },
            (Self::Natural(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -a.pow(&b.into()),
                Negative::Natural(b) => -a.pow(&b),
                Negative::Rational(b) => -Rational::from(a).pow(&b),
            },
            (Self::Rational(a), Self::Negative(i)) => match i {
                Negative::Base(b) => -a.pow(&b.into()),
                Negative::Natural(b) => -a.pow(&b.into()),
                Negative::Rational(b) => -a.pow(&b),
            },
            (Self::Negative(i), Self::Base(b)) => match i {
                Negative::Base(a) => b.pow(&a),
                Negative::Natural(a) => Natural::from(b).pow(&a),
                Negative::Rational(a) => Rational::from(b).pow(&a),
            },
            (Self::Negative(i), Self::Natural(b)) => match i {
                Negative::Base(a) => b.pow(&a.into()),
                Negative::Natural(a) => b.pow(&a),
                Negative::Rational(a) => Rational::from(b).pow(&a),
            },
            (Self::Negative(i), Self::Rational(b)) => match i {
                Negative::Base(a) => b.pow(&a.into()),
                Negative::Natural(a) => b.pow(&a.into()),
                Negative::Rational(a) => b.pow(&a),
            },
            (Self::Negative(a), Self::Negative(b)) => a.pow(&b),
            (Self::Base(a), Self::Imag(b)) => Complex::from(a).pow(&b.into()),
            (Self::Natural(a), Self::Imag(b)) => Complex::from(a).pow(&b.into()),
            (Self::Rational(a), Self::Imag(b)) => Complex::from(a).pow(&b.into()),
            (Self::Negative(a), Self::Imag(b)) => Complex::from(a).pow(&b.into()),
            (Self::Imag(a), Self::Base(b)) => Complex::from(a).pow(&b.into()),
            (Self::Imag(a), Self::Natural(b)) => Complex::from(a).pow(&b.into()),
            (Self::Imag(a), Self::Rational(b)) => Complex::from(a).pow(&b.into()),
            (Self::Imag(a), Self::Negative(b)) => Complex::from(a).pow(&b.into()),
            (Self::Imag(a), Self::Imag(b)) => a.pow(&b),
            (Self::Base(a), Self::Complex(b)) => Complex::from(a).pow(&b),
            (Self::Natural(a), Self::Complex(b)) => Complex::from(a).pow(&b),
            (Self::Rational(a), Self::Complex(b)) => Complex::from(a).pow(&b),
            (Self::Negative(a), Self::Complex(b)) => Complex::from(a).pow(&b),
            (Self::Imag(a), Self::Complex(b)) => Complex::from(a).pow(&b),
            (Self::Complex(a), Self::Base(b)) => a.pow(&b.into()),
            (Self::Complex(a), Self::Natural(b)) => a.pow(&b.into()),
            (Self::Complex(a), Self::Rational(b)) => a.pow(&b.into()),
            (Self::Complex(a), Self::Negative(b)) => a.pow(&b.into()),
            (Self::Complex(a), Self::Imag(b)) => a.pow(&b.into()),
            (Self::Complex(a), Self::Complex(b)) => a.pow(&b),
            (Self::NaN, _) | (_, Self::NaN) => Self::NaN,
        }
    }
}
