#ifndef __TUSB_CONFIG_H__
#define __TUSB_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_TUSB_MCU OPT_MCU_SAMD21
#define CFG_TUSB_OS OPT_OS_NONE

#define CFG_TUD_MIDI 1
#define CFG_TUD_ENDPOINT0_SIZE 64
#define CFG_TUD_MIDI_RX_BUFSIZE 64
#define CFG_TUD_MIDI_TX_BUFSIZE 64
#define CFG_TUSB_MEM_ALIGN __attribute__ ((aligned(4)))

#define BOARD_DEVICE_RHPORT_SPEED OPT_MODE_FULL_SPEED
#define BOARD_DEVICE_RHPORT_NUM 0
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)

#ifdef __cplusplus
}
#endif

#endif
