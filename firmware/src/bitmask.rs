use core::ops::{BitAnd, BitOr, BitXor, Not};

pub trait BitStorage:
    Copy
    + PartialEq
    + BitAnd<Output = Self>
    + BitOr<Output = Self>
    + BitXor<Output = Self>
    + Not<Output = Self>
{
    const ZERO: Self;
}

macro_rules! impl_bit_storage {
    ($($t:ty),*) => {
        $(impl BitStorage for $t {
            const ZERO: Self = 0;
        })*
    };
}

impl_bit_storage!(u8, u16, u32, u64);

pub fn set<T: BitStorage>(value: T, mask: T) -> T {
    value | mask
}

pub fn clear<T: BitStorage>(value: T, mask: T) -> T {
    value & !mask
}

pub fn toggle<T: BitStorage>(value: T, mask: T) -> T {
    value ^ mask
}

pub fn is_set<T: BitStorage>(value: T, mask: T) -> bool {
    value & mask == mask
}

pub fn assign<T: BitStorage>(value: T, mask: T, enabled: bool) -> T {
    if enabled {
        set(value, mask)
    } else {
        clear(value, mask)
    }
}

#[cfg(all(test, not(target_os = "none")))]
mod tests {
    use super::*;

    #[test]
    fn set_clear_toggle_u8() {
        assert_eq!(set(0b0000_0000u8, 0b0000_0011), 0b0000_0011);
        assert_eq!(clear(0b1111_1111u8, 0b0000_0011), 0b1111_1100);
        assert_eq!(toggle(0b1010_1010u8, 0b1111_0000), 0b0101_1010);
    }

    #[test]
    fn is_set_requires_all_bits() {
        assert!(is_set(0b0000_0111u8, 0b0000_0011));
        assert!(!is_set(0b0000_0001u8, 0b0000_0011));
    }

    #[test]
    fn assign_toggles_based_on_flag() {
        assert_eq!(assign(0u8, 0b0001, true), 0b0001);
        assert_eq!(assign(0b0001u8, 0b0001, false), 0b0000);
    }

    #[test]
    fn works_across_int_widths() {
        assert_eq!(set(0u32, 0x8000_0000), 0x8000_0000);
        assert_eq!(clear(0xFFFFu16, 0x00FF), 0xFF00);
    }
}
