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
}
