#ifndef __TUSB_CONFIG_H__
#define __TUSB_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_TUSB_MCU OPT_MCU_SAMD21
#define CFG_TUD_MIDI 1

#define CFG_TUD_MIDI_RX_BUFSIZE 64
#define CFG_TUD_MIDI_TX_BUFSIZE 64

#define BOARD_DEVICE_RHPORT_SPEED OPT_MODE_FULL_SPEED
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)

#ifdef __cplusplus
}
#endif

#endif
