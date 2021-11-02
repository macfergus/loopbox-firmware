#include <cstddef>
#include <cstdint>

#include "tusb.h"

#include "../mcu/samd.h"
#include "./usb.h"

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
}

void usbReset() {
    USB->CTRLA = USB_CTRLA_SWRST;
    while (USB->SYNCBUSY & USB_SYNCBUSY_SWRST) {}
}

void usbEnable() {
    USB->CTRLA = USB_CTRLA_ENABLE;
    while (USB->SYNCBUSY & USB_SYNCBUSY_ENABLE) {}
}

size_t my_strlen(char const* s) {
    size_t count = 0;
    while (*(s + count) != 0) {
        ++count;
    }
    return count;
}

template<typename T>
T max(T first, T second) {
    if (first > second) {
        return first;
    }
    return second;
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

/** out must contain 8 bytes of memory */
void word_to_hex(uint32_t word, char* out) {
    for (int nibble = 0; nibble < 8; ++nibble) {
        out[nibble] = to_hex(word & 0xf);
        word = word >> 4;
    }
}

void get_serial_hex(char* out, size_t n_bytes) {
    uint32_t serial[4];
    read_chip_serial(serial);

    size_t i = 0;
    size_t j = 0;
    for (j = 0; j < 4 && n_bytes >= 9; ++j) {
        word_to_hex(serial[j], out + i);
        n_bytes -= 8;
        i += 8;
    }
    out[i] = 0;
}

void pack_descriptor_string(char const* in, uint16_t* out, size_t bufsize) {
    const auto in_size = my_strlen(in);
    // First word of the buffer is reserved for ID & length
    const auto size = max(in_size, bufsize - 1);
    out[0] = (TUSB_DESC_STRING << 8) | (2 * (size + 1));
    for (size_t i = 0; i < size; ++i) {
        out[i + 1] = uint16_t(in[i]);
    }
}

const size_t DESC_BUF_SIZE = 33;
uint16_t language_descriptor[2];
uint16_t mfr_descriptor[DESC_BUF_SIZE];
uint16_t product_descriptor[DESC_BUF_SIZE];
uint16_t serial_descriptor[DESC_BUF_SIZE];
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    // index:
    // 0 -- supported language
    // 1 -- manufacturer
    // 2 -- product
    // 3 -- serial

    language_descriptor[0] = (TUSB_DESC_STRING << 8) | 2;
    language_descriptor[1] = 0x0409; // english
    pack_descriptor_string("LoopBox", mfr_descriptor, DESC_BUF_SIZE);
    pack_descriptor_string("LoopBox", product_descriptor, DESC_BUF_SIZE);
    char serial[DESC_BUF_SIZE];
    get_serial_hex(serial, DESC_BUF_SIZE);
    pack_descriptor_string(serial, serial_descriptor, DESC_BUF_SIZE);

    if (index == 0) {
        return language_descriptor;
    } else if (index == 1) {
        return mfr_descriptor;
    } else if (index == 2) {
        return product_descriptor;
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
