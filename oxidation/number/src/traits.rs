use core::{fmt, ops};

use crate::number::Number;

pub trait NumberBase:
    Sized
    + num_traits::NumOps<Rhs = Self, Output = Number>
    + num_traits::pow::Pow<Self, Output = Number>
    + ops::BitAnd<Output = Number>
    + ops::BitOr<Output = Number>
    + ops::BitXor<Output = Number>
    + ops::Neg<Output = Number>
    + ops::Not<Output = Number>
    + ops::Shl<Output = Number>
    + ops::Shr<Output = Number>
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
    fn abs(&self) -> Number;
    fn gcd(&self, other: &Self) -> Number;
}
