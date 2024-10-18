#![no_std]
#![no_main]

use cortex_m_rt::entry;
use defmt_rtt as _;
use embedded_hal::digital::OutputPin;
use embedded_time::fixed_point::FixedPoint;
use panic_probe as _;
use rp2040_hal as hal;

use hal::{
    clocks::{init_clocks_and_plls, Clock},
    pac,
    watchdog::Watchdog,
    Sio,
};

#[link_section = ".boot2"]
#[used]
pub static BOOT2: [u8; 256] = rp2040_boot2::BOOT_LOADER_W25Q080;

#[entry]
fn main() -> ! {
    // The following code is responsible for setting up the clocks and the watchdog
    // of the RP2040. It is necessary to run this code before using any of the
    // peripherals of the RP2040.
    let mut pac = pac::Peripherals::take().unwrap();
    let core = pac::CorePeripherals::take().unwrap();
    let mut watchdog = Watchdog::new(pac.WATCHDOG);
    let sio = Sio::new(pac.SIO);

    let external_xtal_freq_hz = 12_000_000u32;
    let clocks = init_clocks_and_plls(
        external_xtal_freq_hz,
        pac.XOSC,
        pac.CLOCKS,
        pac.PLL_SYS,
        pac.PLL_USB,
        &mut pac.RESETS,
        &mut watchdog,
    )
    .ok()
    .unwrap();

    let delay = cortex_m::delay::Delay::new(core.SYST, clocks.system_clock.freq().to_Hz());

    let pins = hal::gpio::Pins::new(
        pac.IO_BANK0,
        pac.PADS_BANK0,
        sio.gpio_bank0,
        &mut pac.RESETS,
    );

    // Place your pin definitions here
    // let mut led_pin = pins.gpio25.into_push_pull_output();
    let tx_pin = pins.gpio16.into_push_pull_output();

    // Place the rest of your setup code here
    let transmitter = Transmitter::new(tx_pin, &delay);

    loop {
        // Place your main loop code here
        transmitter.send_bytes(&[0x01, 0x02, 0x03, 0x04]);
        delay.delay_ms(1000);
    }
}


// Radio Transmitter implementatoin

// trait Transmitter {
//     fn send_byte(&self, byte: u8);
//     fn send_bytes(&self, bytes: &[u8]);
//     fn _send_bit(&self, bit: bool);
//     fn _send_start(&self);
//     fn _send_end(&self);
// }

struct Transmitter <'a, IoPin: hal::gpio::PinId> {
    tx_pin: hal::gpio::Pin<IoPin, hal::gpio::FunctionSio<hal::gpio::SioOutput>, hal::gpio::PullDown>,
    delay: &'a cortex_m::delay::Delay,
}

impl<'a, IoPin: hal::gpio::PinId> Transmitter<'a, IoPin> {
    fn new(tx_pin: hal::gpio::Pin<IoPin, hal::gpio::FunctionSio<hal::gpio::SioOutput>, hal::gpio::PullDown>, delay: &'a cortex_m::delay::Delay) -> Self {
        Self {
            tx_pin,
            delay,
        }
    }

    fn _send_byte(&self, byte: u8) {
        // let encoded = self._hamming_encode(byte);
        for i in 0..12 {
            self._send_bit((byte >> i) & 1 == 1);
        }
    }

    // Even Parity Hamming 12,8 encoding
    // P0 = D0 + D1 + D3 + D4 + D6
    // P1 = D0 + D2 + D3 + D5 + D6
    // P2 = D1 + D2 + D3 + D7
    // P3 = D4 + D5 + D6 + D7
    //
    // Encoded value:
    // D7 D6 D5 D4 P3 D3 D2 D1 P2 D0 P1 P0
    // fn _hamming_encode(&mut self, byte: u8) -> u16 {
    //     let mut encoded: u16 = 0;
    //     // Put data bits in correct encoded place
    //     encoded |= ((byte & 0b11110000) as u16) << 4;
    //     encoded |= ((byte & 0b00001110) as u16) << 3;
    //     encoded |= ((byte & 0b00000001) as u16) << 2;
        
    //     let mut parity: u16 = 0;
        
    //     for parity_num in 0..4 {
    //         let max_size = 12;
    //         let parity_position = 1 << parity_num;
    //         let zero_parity_position = parity_position - 1;
    //         let mut parity = 0;
    //         for start_bit_position in (zero_parity_position..max_size).step_by(parity_position*2) {
    //             let end_bit_position = usize::min(start_bit_position+parity_position,max_size);
    //             for i in start_bit_position..end_bit_position {
    //                 if i == zero_parity_position {
    //                     continue
    //                 }
    //                 parity ^= (encoded >> i) & 1
    //             }
    //         }
    //         encoded |= parity << (parity_position-1)
    //     }
    
    //     return encoded;
    // }

    fn send_bytes(&self, bytes: &[u8]) {

        // Send the preamble
        self._send_start();

        let mut crc: u8 = 0;

        // Send the bytes
        for byte in bytes {
            
            crc ^= *byte;

            // Send the Byte
            self._send_byte(*byte);
            // End of Byte
            self._send_bit(true);
        }

        // Send the CRC
        self._send_byte(crc);

        // Send the end
        self._send_end();
    }

    fn _send_bit(&self, bit: bool) {
        self.tx_pin.set_high().unwrap();
        if bit {
            self.tx_pin.set_low().unwrap();
        }
    }

    // Sends the preamble
    fn _send_start(&self) {
        self.tx_pin.set_high().unwrap();
        self.delay.delay_us(15000);
        self.tx_pin.set_low().unwrap();
        self.delay.delay_us(400);
        self.tx_pin.set_high().unwrap();
        self.delay.delay_us(400);
    }

    fn _send_end(&self) {
        self.tx_pin.set_low().unwrap();
        self.delay.delay_us(798);
    }
}