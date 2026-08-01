#![no_std]
#![no_main]

mod hts221;

use cortex_m_rt::entry;
use panic_rtt_target as _;
use rtt_target::{rprintln, rtt_init_print};
use stm32f4xx_hal::{i2c::I2c, pac, prelude::*};

use hts221::Hts221;

#[entry]
fn main() -> ! {
    rtt_init_print!();

    let dp = pac::Peripherals::take().unwrap();
    let rcc = dp.RCC.constrain();
    let clocks = rcc.cfgr.sysclk(84.MHz()).freeze();
    let gpiob = dp.GPIOB.split();
    let scl = gpiob.pb8.into_alternate_open_drain();
    let sda = gpiob.pb9.into_alternate_open_drain();
    let mut i2c = I2c::new(dp.I2C1, (scl, sda), 100.kHz(), &clocks);

    let sensor = match Hts221::init(&mut i2c) {
        Ok(s) => {
            rprintln!("HTS221 initialized OK");
            s
        }
        Err(_) => {
            rprintln!("HTS221 init failed (bad WHO_AM_I or I2C error)");
            loop {
                cortex_m::asm::nop();
            }
        }
    };

    loop {
        if let Ok(Some(reading)) = sensor.read(&mut i2c) {
            rprintln!(
                "temp: {} C, humidity: {} %RH",
                reading.temperature_c,
                reading.humidity_rh
            );
        }
    }
}
