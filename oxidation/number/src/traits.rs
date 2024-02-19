#![deny(clippy::pedantic)]
#![deny(clippy::restriction)]
#![allow(clippy::arithmetic_side_effects)]
#![allow(clippy::blanket_clippy_restriction_lints)]
#![allow(clippy::implicit_return)]
#![allow(clippy::missing_docs_in_private_items)]

use core::fmt::{Binary, Debug, Display, LowerExp, LowerHex, Octal, Pointer, UpperExp, UpperHex};
use core::ops::{Add, BitAnd, BitOr, BitXor, Div, Mul, Neg, Not, Rem, Shl, Shr, Sub};
use num_traits::Pow;

use crate::number::Number;
use crate::utils::gcd;

pub trait NumberBase:
    Sized
    + Into<Number>
    + num_traits::ToPrimitive
    + Pow<Self, Output = Number>
    // + num_integer::Integer<Output = Number>
    + Add<Output = Number>
    + BitAnd<Output = Number>
    + BitOr<Output = Number>
    + BitXor<Output = Number>
    + Div<Output = Number>
    + Mul<Output = Number>
    + Neg<Output = Number>
    + Not<Output = Number>
    + Rem<Output = Number>
    + Shl<Output = Number>
    + Shr<Output = Number>
    + Sub<Output = Number>
    + Binary
    + Debug
    + Display
    + LowerExp
    + LowerHex
    + Octal
    + Pointer
    + UpperExp
    + UpperHex
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
