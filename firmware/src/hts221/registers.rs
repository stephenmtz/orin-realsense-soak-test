pub(crate) const DEVICE_ADDR: u8 = 0x5F;

pub(crate) const AUTO_INCREMENT: u8 = 0x80;

pub(crate) const WHO_AM_I: u8 = 0x0F;
pub(crate) const CTRL_REG1: u8 = 0x20;
pub(crate) const STATUS_REG: u8 = 0x27;
pub(crate) const HUMIDITY_OUT_L: u8 = 0x28;
pub(crate) const TEMP_OUT_L: u8 = 0x2A;

// Humidity calibration
pub(crate) const H0_RH_X2: u8 = 0x30;
pub(crate) const H1_RH_X2: u8 = 0x31;
pub(crate) const H0_T0_OUT_L: u8 = 0x36;
pub(crate) const H1_T0_OUT_L: u8 = 0x3A;

// Temperature calibration
pub(crate) const T0_DEGC_X8: u8 = 0x32;
pub(crate) const T1_DEGC_X8: u8 = 0x33;
pub(crate) const T0_T1_MSB: u8 = 0x35;
pub(crate) const T0_OUT_L: u8 = 0x3C;
pub(crate) const T1_OUT_L: u8 = 0x3E;
