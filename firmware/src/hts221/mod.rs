use stm32f4xx_hal::i2c::I2c;
use stm32f4xx_hal::pac::I2C1;

mod registers;
use registers::*;

pub struct Hts221 {
    cal: Calibration,
}

struct Calibration {
    h0_rh: f32,
    h1_rh: f32,
    h0_t0_out: i16,
    h1_t0_out: i16,
    t0_degc: f32,
    t1_degc: f32,
    t0_out: i16,
    t1_out: i16,
}

pub struct Reading {
    pub temperature_c: f32,
    pub humidity_rh: f32,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Error {
    Bus,
    UnexpectedDeviceId,
}

impl Hts221 {
    pub fn init(i2c: &mut I2c<I2C1>) -> Result<Self, Error> {
        let who = read_reg(i2c, WHO_AM_I)?;
        if who != 0xBC {
            return Err(Error::UnexpectedDeviceId);
        }

        write_reg(i2c, CTRL_REG1, 0b1000_0101)?;

        let cal = read_calibration(i2c)?;
        Ok(Self { cal })
    }

    pub fn read(&self, i2c: &mut I2c<I2C1>) -> Result<Option<Reading>, Error> {
        let status = read_reg(i2c, STATUS_REG)?;
        if status & 0b11 != 0b11 {
            return Ok(None);
        }

        let h_out = read_i16(i2c, HUMIDITY_OUT_L)?;
        let t_out = read_i16(i2c, TEMP_OUT_L)?;

        Ok(Some(Reading {
            temperature_c: self.cal.temperature_c(t_out),
            humidity_rh: self.cal.humidity_rh(h_out),
        }))
    }
}

impl Calibration {
    fn temperature_c(&self, t_out: i16) -> f32 {
        let slope = (self.t1_degc - self.t0_degc) / (self.t1_out - self.t0_out) as f32;
        self.t0_degc + slope * (t_out - self.t0_out) as f32
    }

    fn humidity_rh(&self, h_out: i16) -> f32 {
        let slope = (self.h1_rh - self.h0_rh) / (self.h1_t0_out - self.h0_t0_out) as f32;
        self.h0_rh + slope * (h_out - self.h0_t0_out) as f32
    }
}

fn read_calibration(i2c: &mut I2c<I2C1>) -> Result<Calibration, Error> {
    let h0_rh = read_reg(i2c, H0_RH_X2)? as f32 / 2.0;
    let h1_rh = read_reg(i2c, H1_RH_X2)? as f32 / 2.0;
    let h0_t0_out = read_i16(i2c, H0_T0_OUT_L)?;
    let h1_t0_out = read_i16(i2c, H1_T0_OUT_L)?;

    let t0_t1_msb = read_reg(i2c, T0_T1_MSB)?;
    let t0_degc_x8 = read_reg(i2c, T0_DEGC_X8)? as u16 | (((t0_t1_msb & 0b0011) as u16) << 8);
    let t1_degc_x8 = read_reg(i2c, T1_DEGC_X8)? as u16 | (((t0_t1_msb & 0b1100) as u16) << 6);
    let t0_out = read_i16(i2c, T0_OUT_L)?;
    let t1_out = read_i16(i2c, T1_OUT_L)?;

    Ok(Calibration {
        h0_rh,
        h1_rh,
        h0_t0_out,
        h1_t0_out,
        t0_degc: t0_degc_x8 as f32 / 8.0,
        t1_degc: t1_degc_x8 as f32 / 8.0,
        t0_out,
        t1_out,
    })
}

fn read_reg(i2c: &mut I2c<I2C1>, reg: u8) -> Result<u8, Error> {
    let mut buf = [0u8; 1];
    i2c.write_read(DEVICE_ADDR, &[reg], &mut buf)
        .map_err(|_| Error::Bus)?;
    Ok(buf[0])
}

fn write_reg(i2c: &mut I2c<I2C1>, reg: u8, val: u8) -> Result<(), Error> {
    i2c.write(DEVICE_ADDR, &[reg, val]).map_err(|_| Error::Bus)
}

fn read_i16(i2c: &mut I2c<I2C1>, reg_l: u8) -> Result<i16, Error> {
    let mut buf = [0u8; 2];
    i2c.write_read(DEVICE_ADDR, &[reg_l | AUTO_INCREMENT], &mut buf)
        .map_err(|_| Error::Bus)?;
    Ok(i16::from_le_bytes(buf))
}
