#include <cstddef>
#include <cstdint>

#include "./systick.h"
#include "./usb.h"

// The "practical" entrypoint for your program
extern int main();

// The linker script must define these values
extern uint32_t __stack_top__;
// This is where global data comes from (in flash??)
extern uint32_t __etext__;
// This is where global data lands (in sram??)
extern uint32_t __data_start__;
extern uint32_t __data_end__;
// This is where globals that should be zeroed out go
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

// The start of memory must contain this interrupt vector table. The processor
// will use it to figure out where to start execution
// Ref: https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-processor/exception-model/vector-table
using InterruptHandler = void (*)();

struct isr_vectors {
    // Pointer to the stack
    void* stack; // 0x0
    // This is were code starts execution
    InterruptHandler reset; // 0x04
    // The rest are interrupt handlers
    InterruptHandler nmi; // 0x08
    InterruptHandler hard_fault; // 0x0c
    InterruptHandler mem_mgmt_fault; // 0x10
    InterruptHandler bus_fault; // 0x14
    InterruptHandler usage_fault; // 0x18
    InterruptHandler reserved1; // 0x1c
    InterruptHandler reserved2; // 0x20
    InterruptHandler reserved3; // 0x24
    InterruptHandler reserved4; // 0x28
    InterruptHandler sv_call; // 0x2c
    InterruptHandler reserved_debug; // 0x30
    InterruptHandler reserved_debug2; // 0x34
    InterruptHandler pend_sv; // 0x38
    InterruptHandler systick; // 0x3c

    InterruptHandler pm; // irq 0
    InterruptHandler sysctrl; // irq 1
    InterruptHandler wdt; // irq 2
    InterruptHandler rtc; // irq 3
    InterruptHandler eic; // irc 4
    InterruptHandler nvmctrl; // irq 5
    InterruptHandler dmac; // irq 6
    InterruptHandler usb; // irq 7
    InterruptHandler evsys; // irq 8
};

// Do-nothing interrupt handler
void dummyHandler() {
    return;
}

// Interrupt handler that freezes the CPU
void freeze() {
    while (1) {}
}

// This is where execution starts. For our simple program, we just jump right
// into main()
// In a more complicated program, you might have to do some system setup, or
// initialize globals (e.g., copying initial values from rom, calling
// constructors, etc)
extern "C" void __libc_init_array(void);
extern "C" {
void resetHandler() {
    uint32_t *dest;
    uint32_t *src;

    // Copy the data section
    dest = &__data_start__;
    src = &__etext__;
    // why is this condition necessary???
    if (src != dest) {
        while (dest != &__data_end__) {
            *dest = *src;
            ++dest;
            ++src;
        }
    }

    // Zero out .bss section
    dest = &__bss_start__;
    while (dest != &__bss_end__) {
        *dest = 0;
        ++dest;
    }

    // Call ctors
    __libc_init_array();

    main();

    // If main ever exits, just spin afterwards
    while (true) {}
}
}

// This table needs to live in a specific place in memory.
// __attribute__ ((section)) marks this struct for special handling in the
// linker.
// Then your linker script, you can specify the memory location for the
// .isr_vector section.
__attribute__ ((section(".isr_vectors"), used))
const struct isr_vectors interrupt_vector_table = {
    .stack = &__stack_top__,
    .reset = resetHandler,
    .nmi = freeze,
    .hard_fault = freeze,
    .mem_mgmt_fault = freeze,
    .bus_fault = freeze,
    .usage_fault = freeze,
    .reserved1 = nullptr,
    .reserved2 = nullptr,
    .reserved3 = nullptr,
    .reserved4 = nullptr,
    .sv_call = freeze,
    .reserved_debug = nullptr,
    .pend_sv = nullptr,
    .systick = handleSystick,
    .pm = freeze,
    .sysctrl = freeze,
    .wdt = dummyHandler,
    .rtc = dummyHandler,
    .eic = dummyHandler,
    .nvmctrl = dummyHandler,
    .dmac = dummyHandler,
    .usb = handleUSB,
    .evsys = dummyHandler,
};
