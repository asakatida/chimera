use core::{fmt, ops};

use crate::number::Number;
use crate::utils::gcd;

pub trait NumberBase:
    Sized
    + Into<Number>
    + num_traits::ToPrimitive
    + num_traits::pow::Pow<Self, Output = Number>
    // + num_integer::Integer<Output = Number>
    + ops::Add<Output = Number>
    + ops::BitAnd<Output = Number>
    + ops::BitOr<Output = Number>
    + ops::BitXor<Output = Number>
    + ops::Div<Output = Number>
    + ops::Mul<Output = Number>
    + ops::Neg<Output = Number>
    + ops::Not<Output = Number>
    + ops::Rem<Output = Number>
    + ops::Shl<Output = Number>
    + ops::Shr<Output = Number>
    + ops::Sub<Output = Number>
    + fmt::Binary
    + fmt::Debug
    + fmt::Display
    + fmt::LowerExp
    + fmt::LowerHex
    + fmt::Octal
    + fmt::Pointer
    + fmt::UpperExp
    + fmt::UpperHex
{
    #[inline]
    #[must_use]
    fn abs(self) -> Number {
        self.into()
    }
    fn div_floor(self, other: Self) -> Number;
    #[inline]
    #[must_use]
    fn gcd(self, other: Self) -> Number {
        gcd(self, other)
    }
    #[inline]
    #[must_use]
    fn mod_pow(self, exp: Self, modu: Self) -> Number {
        self.pow(exp) % modu.into()
    }
}
