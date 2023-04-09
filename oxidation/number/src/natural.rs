#![allow(clippy::todo)]

use core::{cmp, fmt, ops};

use crate::base::Base;
use crate::negative::Negative;
use crate::number::Number;
use crate::rational::{Part, Rational};
use crate::traits::NumberBase;
use crate::utils::{fmt_ptr, gcd, rem};

#[non_exhaustive]
#[derive(Clone, Debug, Default, Eq, Ord, PartialEq, PartialOrd)]
pub struct Natural {
    pub value: Vec<u64>,
}

#[allow(clippy::missing_trait_methods)]
impl PartialEq<u64> for Natural {
    #[inline]
    fn eq(&self, _other: &u64) -> bool {
        false
    }
}

#[allow(clippy::missing_trait_methods)]
impl PartialOrd<u64> for Natural {
    #[inline]
    fn partial_cmp(&self, _other: &u64) -> Option<cmp::Ordering> {
        Some(cmp::Ordering::Greater)
    }
}

impl From<u64> for Natural {
    #[inline]
    fn from(i: u64) -> Self {
        Natural { value: vec![i] }
    }
}
impl From<Base> for Natural {
    #[inline]
    fn from(i: Base) -> Self {
        (&i).into()
    }
}
impl From<&Base> for Natural {
    #[inline]
    fn from(i: &Base) -> Self {
        Natural {
            value: vec![i.value],
        }
    }
}

impl Natural {
    #[inline]
    #[must_use]
    pub fn as_f64(self) -> f64 {
        if self.value.len() > 2 {
            f64::INFINITY
        } else {
            f64::MAX
        }
    }
}

impl fmt::Binary for Natural {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.value
            .iter()
            .rev()
            .try_for_each(|i| fmt::Binary::fmt(i, f))
    }
}

impl fmt::Display for Natural {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.value
            .iter()
            .rev()
            .try_for_each(|i| fmt::Display::fmt(i, f))
    }
}

impl fmt::LowerExp for Natural {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.value
            .iter()
            .rev()
            .try_for_each(|i| fmt::LowerExp::fmt(i, f))
    }
}

impl fmt::LowerHex for Natural {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.value
            .iter()
            .rev()
            .try_for_each(|i| fmt::LowerHex::fmt(i, f))
    }
}

impl fmt::Octal for Natural {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.value
            .iter()
            .rev()
            .try_for_each(|i| fmt::Octal::fmt(i, f))
    }
}

impl fmt::Pointer for Natural {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt_ptr(self, f)
    }
}

impl fmt::UpperExp for Natural {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.value
            .iter()
            .rev()
            .try_for_each(|i| fmt::UpperExp::fmt(i, f))
    }
}

impl fmt::UpperHex for Natural {
    #[inline]
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.value
            .iter()
            .rev()
            .try_for_each(|i| fmt::UpperHex::fmt(i, f))
    }
}

impl ops::Add for Natural {
    type Output = Number;

    #[inline]
    fn add(self, other: Self) -> Self::Output {
        let mut left = self.value;
        let mut right = other.value;
        let len = cmp::max(left.len(), right.len());
        left.resize(len, 0);
        right.resize(len, 0);
        let mut intermediate: Vec<_> = left
            .iter()
            .zip(right.iter())
            .map(|(i, j)| i.overflowing_add(*j))
            .collect();
        while intermediate.iter().any(|&(_, carry)| carry) {
            let mut overflow = vec![0];
            overflow.extend(intermediate.iter().map(|i| u64::from(i.1)));
            intermediate.resize(overflow.len(), (0, false));
            intermediate = overflow
                .iter()
                .zip(intermediate.iter().map(|&(i, _)| i))
                .map(|(&i, j)| i.overflowing_add(j))
                .collect();
        }
        Self {
            value: intermediate.iter().map(|&(i, _)| i).collect(),
        }
        .into()
    }
}

impl ops::BitAnd for Natural {
    type Output = Number;

    #[inline]
    fn bitand(self, other: Self) -> Self::Output {
        Self {
            value: self
                .value
                .iter()
                .zip(other.value.iter())
                .map(|(i, j)| i & j)
                .collect(),
        }
        .into()
    }
}

impl ops::BitOr for Natural {
    type Output = Number;

    #[inline]
    fn bitor(self, other: Self) -> Self::Output {
        let mut left = self.value;
        let mut right = other.value;
        let len = cmp::max(left.len(), right.len());
        left.resize(len, 0);
        right.resize(len, 0);
        Self {
            value: left.iter().zip(right.iter()).map(|(i, j)| i | j).collect(),
        }
        .into()
    }
}

impl ops::BitXor for Natural {
    type Output = Number;

    #[inline]
    fn bitxor(self, other: Self) -> Self::Output {
        let mut left = self.value;
        let mut right = other.value;
        let len = cmp::max(left.len(), right.len());
        left.resize(len, 0);
        right.resize(len, 0);
        Self {
            value: left.iter().zip(right.iter()).map(|(i, j)| i ^ j).collect(),
        }
        .into()
    }
}

impl ops::Div for Natural {
    type Output = Number;

    #[inline]
    fn div(self, other: Self) -> Number {
        Rational {
            numerator: Part::Natural(self),
            denominator: Part::Natural(other),
        }
        .into()
    }
}

impl ops::Mul for Natural {
    type Output = Number;

    #[inline]
    fn mul(self, other: Self) -> Self::Output {
        let mut left = self.value;
        let mut right = other.value;
        {
            let len = cmp::max(left.len(), right.len());
            left.resize(len, 0);
            right.resize(len, 0);
        };
        Self {
            value: left
                .iter()
                .enumerate()
                .map(|(pos, i)| {
                    let mut sum = vec![(0, false); pos];
                    sum.extend(
                        right
                            .iter()
                            .map(|j| u128::from(*i).overflowing_mul((*j).into())),
                    );
                    sum
                })
                .fold(vec![], |mut sum, next_raw| {
                    if next_raw.iter().any(|&(_, carry)| carry) {
                        todo!();
                    }
                    let next_big: Vec<_> = next_raw.iter().map(|&(i, _)| i).collect();
                    let mut next: Vec<_> = next_big
                        .iter()
                        .map(|i| u64::try_from(i & u128::from(u64::MAX)).unwrap_or_default())
                        .collect();
                    let len = cmp::max(sum.len(), next.len());
                    sum.resize(len, 0);
                    next.resize(len, 0);
                    let mut intermediate: Vec<_> = sum
                        .iter()
                        .zip(next.iter())
                        .map(|(i, j)| i.overflowing_add(*j))
                        .collect();
                    while intermediate.iter().any(|&(_, carry)| carry) {
                        let mut overflow = vec![0];
                        overflow.extend(intermediate.iter().map(|i| u64::from(i.1)));
                        intermediate.resize(overflow.len(), (0, false));
                        intermediate = overflow
                            .iter()
                            .zip(intermediate.iter().map(|&(i, _)| i))
                            .map(|(&i, j)| i.overflowing_add(j))
                            .collect();
                    }
                    intermediate.iter().map(|&(i, _)| i).collect()
                }),
        }
        .into()
    }
}

impl ops::Neg for Natural {
    type Output = Number;

    #[inline]
    fn neg(self) -> Self::Output {
        Negative::Natural(self).into()
    }
}

impl ops::Not for Natural {
    type Output = Number;

    #[inline]
    fn not(self) -> Self::Output {
        Self {
            value: self.value.iter().map(|i| !i).collect(),
        }
        .into()
    }
}

impl ops::Rem for Natural {
    type Output = Number;

    #[inline]
    fn rem(self, other: Self) -> Self::Output {
        rem(self, other)
    }
}

impl ops::Shl for Natural {
    type Output = Number;

    #[inline]
    fn shl(self, other: Self) -> Self::Output {
        let mut left = self.value;
        let mut right = other.value;
        let len = cmp::max(left.len(), right.len());
        left.resize(len, 0);
        right.resize(len, 0);
        Self {
            value: left.iter().zip(right.iter()).map(|(i, j)| i << j).collect(),
        }
        .into()
    }
}

impl ops::Shr for Natural {
    type Output = Number;

    #[inline]
    fn shr(self, other: Self) -> Self::Output {
        let mut left = self.value;
        let mut right = other.value;
        let len = cmp::max(left.len(), right.len());
        left.resize(len, 0);
        right.resize(len, 0);
        Self {
            value: left.iter().zip(right.iter()).map(|(i, j)| i >> j).collect(),
        }
        .into()
    }
}

impl ops::Sub for Natural {
    type Output = Number;

    #[inline]
    fn sub(self, other: Self) -> Self::Output {
        if self < other {
            let mut left = self.value;
            let mut right = other.value;
            let len = cmp::max(left.len(), right.len());
            left.resize(len, 0);
            right.resize(len, 0);
            let mut intermediate: Vec<_> = left
                .iter()
                .zip(right.iter())
                .map(|(i, j)| i.overflowing_sub(*j))
                .collect();
            while intermediate.iter().any(|&(_, carry)| carry) {
                let mut overflow = vec![0];
                overflow.extend(intermediate.iter().map(|i| u64::from(i.1)));
                intermediate.resize(overflow.len(), (0, false));
                intermediate = overflow
                    .iter()
                    .zip(intermediate.iter().map(|&(i, _)| i))
                    .map(|(&i, j)| i.overflowing_sub(j))
                    .collect();
            }
            Self {
                value: intermediate.iter().map(|&(i, _)| i).collect(),
            }
            .into()
        } else {
            let mut left = self.value;
            let mut right = other.value;
            let len = cmp::max(left.len(), right.len());
            left.resize(len, 0);
            right.resize(len, 0);
            let mut intermediate: Vec<_> = left
                .iter()
                .zip(right.iter())
                .map(|(i, j)| i.overflowing_sub(*j))
                .collect();
            while intermediate.iter().any(|&(_, carry)| carry) {
                let mut overflow = vec![0];
                overflow.extend(intermediate.iter().map(|i| u64::from(i.1)));
                intermediate.resize(overflow.len(), (0, false));
                intermediate = overflow
                    .iter()
                    .zip(intermediate.iter().map(|&(i, _)| i))
                    .map(|(&i, j)| i.overflowing_sub(j))
                    .collect();
            }
            Negative::Natural(Self {
                value: intermediate.iter().map(|&(i, _)| i).collect(),
            })
            .into()
        }
    }
}

impl NumberBase for Natural {
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
