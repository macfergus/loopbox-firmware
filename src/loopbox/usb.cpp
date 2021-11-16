#include <cstddef>
#include <cstdint>
#include <span>

#include "tusb.h"

#include "../mcu/samd.h"
#include "./usb.h"

const size_t MAX_DESCRIPTOR_LENGTH = 126;

template<size_t N>
struct USBDescriptor {
    constexpr USBDescriptor(char16_t const (&data)[N]) :
        n_bytes(2 * (N - 1) + 2),
        type(TUSB_DESC_STRING) {

        static_assert(N <= MAX_DESCRIPTOR_LENGTH, "descriptor is too long");
        for (size_t i = 0; i < N - 1; ++i) {
            characters[i] = data[i];
        }
    }

    uint8_t n_bytes;
    uint8_t type;
    char16_t characters[N - 1];
};

void waitClockReady() {
    while (!(SYSCTRL->PCLKSR & SYSCTRL_PCLKSR_DFLLRDY)) {
    }
}

void initUSBClock() {
    // before speeding up the CPU clock, need to enable 1 read wait state
    NVM->CTRLB = NVM->CTRLB | (1 << NVM_RWS_OFFSET);

    // enable the DFLL
    waitClockReady();
    SYSCTRL->DFLLCTRL = SYSCTRL_DFLLCTRL_ENABLE;
    waitClockReady();

    // copy DFLL calibration from fuses
    const uint32_t coarse =
        (NVMCAL[1] & NVM_CAL_COARSE_MASK) >> NVM_CAL_COARSE_OFFSET;
    const uint32_t fine =
        (NVMCAL[2] & NVM_CAL_FINE_MASK) >> NVM_CAL_FINE_OFFSET;
    SYSCTRL->DFLLVAL = (
        (coarse << SYSCTRL_DFLLVAL_COARSE_OFFSET) |
        (fine << SYSCTRL_DFLLVAL_FINE_OFFSET)
    );
    waitClockReady();

    // Enable USB clock recovery -- this synchronizes our DFLL to the USB host
    SYSCTRL->DFLLCTRL = SYSCTRL->DFLLCTRL |
        SYSCTRL_DFLLCTRL_USBCRM |
        SYSCTRL_DFLLCTRL_CCDIS;
    // source clock is 1 KHz, scale up to 48 MHz
    SYSCTRL->DFLLMUL =
        (1 << SYSCTRL_DFLLMUL_COARSE_STEP_OFFSET) |
        (1 << SYSCTRL_DFLLMUL_FINE_STEP_OFFSET) |
        48000;
    // switch DFLL to closed loop and enable
    SYSCTRL->DFLLCTRL = SYSCTRL->DFLLCTRL |
        SYSCTRL_DFLLCTRL_MODE |
        SYSCTRL_DFLLCTRL_ENABLE;
    waitClockReady();

    // Now set the DFLL to be the system clock
    GCLK->GENCTRL =
        GCLK_GENCTRL_ID_GEN0 |
        GCLK_GENCTRL_SRC_DFLL48M |
        GCLK_GENCTRL_GENEN |
        GCLK_GENCTRL_IDC;
    while (GCLK->STATUS & GCLK_STATUS_SYNCBUSY) {}

    // Enable the APB/AHB bus clocks
    PM->APBBMASK = PM->APBBMASK | PM_APBBMASK_USB_ENABLE;
    PM->AHBMASK = PM->AHBMASK | PM_AHBMASK_USB_ENABLE;

    // Route clock generator 0 to the USB peripheral and enable it
    GCLK->CLKCTRL =
        GCLK_CLKCTRL_ID_USB |
        GCLK_CLKCTRL_GEN0 |
        GCLK_CLKCTRL_CLKEN;
}

void usbReset() {
    USB->CTRLA = USB_CTRLA_SWRST;
    while (USB->SYNCBUSY & USB_SYNCBUSY_SWRST) {}
}

void usbEnable() {
    USB->CTRLA = USB_CTRLA_ENABLE;
    while (USB->SYNCBUSY & USB_SYNCBUSY_ENABLE) {}
}

void initUSBPins() {
    const uint32_t pin_dm = 24;
    const uint32_t pin_dp = 25;
    PORTA->DIRSET = (1 << pin_dm);
    PORTA->DIRSET = (1 << pin_dp);
    PORTA->PINCFG[pin_dm] = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
    PORTA->PINCFG[pin_dp] = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
    PORTA->PMUX[12] = (PORT_PMUX_G << 4) | PORT_PMUX_G;
}

uint8_t to_hex(uint8_t x) {
    if (x < 10) {
        return '0' + x;
    }
    if (x < 16) {
        return 'a' + (x - 10);
    }
    return '.';
}

auto word_to_hex(uint32_t word, std::span<char> out) {
    uint32_t n_nibbles = 0;
    auto p_out = out.begin();
    const auto end = out.end();
    while (p_out != end && n_nibbles < 8) {
        *p_out = to_hex(word & 0xf);
        word = word >> 4;
        ++p_out;
        ++n_nibbles;
    }
    return p_out;
}

void get_serial_hex(std::span<char> out) {
    for (auto& x : out) {
        x = 0;
    }
    // save room for null terminator
    auto buf = out.first(out.size() - 1);
    auto p_out = buf.begin();
    const auto serial = read_chip_serial();

    p_out = word_to_hex(serial.word1, {p_out, buf.end()});
    p_out = word_to_hex(serial.word2, {p_out, buf.end()});
    p_out = word_to_hex(serial.word3, {p_out, buf.end()});
    p_out = word_to_hex(serial.word4, {p_out, buf.end()});
}

/** Convert an ascii string to utf-16 and add descriptor header */
void pack_descriptor_string(char const* in, std::span<uint16_t> out) {
    char const* p = in;
    // Skip the first byte for the string length
    auto q = std::next(out.begin());
    size_t n_bytes = 0;
    const auto end = out.end();
    while ((*p != 0) && (q != end) && n_bytes < 254) {
        *q = uint16_t(*p);
        ++p;
        ++q;
        n_bytes += 2;
    }
    out.front() = (TUSB_DESC_STRING << 8) | (n_bytes + 2);
}

const size_t DESC_BUF_SIZE = 33;
const uint16_t language_descriptor[2] = {
    (TUSB_DESC_STRING << 8) | 4, // length + type
    0x0409 // english
};
constexpr USBDescriptor mfr_descriptor = { u"LoopBox" };
constexpr USBDescriptor product_descriptor = { u"LoopBox" };
uint16_t serial_descriptor[DESC_BUF_SIZE];
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    // index:
    // 0 -- supported language
    // 1 -- manufacturer
    // 2 -- product
    // 3 -- serial

    char serial[DESC_BUF_SIZE];
    get_serial_hex(serial);
    pack_descriptor_string(serial, serial_descriptor);

    if (index == 0) {
        return language_descriptor;
    } else if (index == 1) {
        return reinterpret_cast<uint16_t const*>(&mfr_descriptor);
    } else if (index == 2) {
        return reinterpret_cast<uint16_t const*>(&product_descriptor);
    } else if (index == 3) {
        return serial_descriptor;
    }
    // 0xee could be a special microsoft thing, but don't try to handle it
    return nullptr;
}

const tusb_desc_device_t device_descriptor = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0x04d8,
    .idProduct          = 0xe87c,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

uint8_t const* tud_descriptor_device_cb() {
    return reinterpret_cast<uint8_t const*>(&device_descriptor);
}

#define ITF_NUM_MIDI 0
#define EPNUM_MIDI 1
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_MIDI_DESC_LEN)

uint8_t const descriptor_conf[] = {
    TUD_CONFIG_DESCRIPTOR(
        1 /* config number */,
        2 /* num interfaces */,
        0 /* string index */,
        CONFIG_TOTAL_LEN,
        0 /* attributes */,
        150 /* power (mA) */
    ),
    TUD_MIDI_DESCRIPTOR(
        ITF_NUM_MIDI,
        0, // string index
        EPNUM_MIDI, // out address
        0x80 | EPNUM_MIDI, // in address
        64 // endpoint size
    )
};

uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    return reinterpret_cast<uint8_t const*>(&descriptor_conf);
}

void handleUSB() {
    tud_int_handler(0);
}
