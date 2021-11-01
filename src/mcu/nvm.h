#ifndef __MCU_NVM_H__
#define __MCU_NVM_H__

struct nvmctrl {
    volatile uint16_t CTRLA;
    volatile uint16_t reserved;
    volatile uint32_t CTRLB;
};
#define NVM ((volatile struct nvmctrl*)(0x41004000))
#define NVMCAL ((volatile uint32_t*)(0x806020))

const uint8_t NVM_RWS_OFFSET = 1;

const uint32_t NVM_CAL_FINE_MASK = 0x3ff;
const uint8_t NVM_CAL_FINE_OFFSET = 0;
const uint32_t NVM_CAL_COARSE_MASK = 0xfc000000;
const uint8_t NVM_CAL_COARSE_OFFSET = 26;

#endif
