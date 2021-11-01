CXX=arm-none-eabi-g++
CC=arm-none-eabi-gcc
LD=arm-none-eabi-g++
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=-g \
    -Og \
    -D__SAMD21G15A__ \
    -Iinclude \
    -fdata-sections -ffunction-sections \
    -mabi=aapcs -flto -mthumb -mcpu=cortex-m0plus
CXXFLAGS=-std=c++20 $(CFLAGS)
LDFLAGS=-flto -mthumb -mcpu=cortex-m0plus --specs=nosys.specs -Wl,--gc-sections

SRCDIR=src
BUILDDIR=build
CPP_OBJDIR=$(BUILDDIR)/cpp
C_OBJDIR=$(BUILDDIR)/c

CPP_SRCS := $(shell find $(SRCDIR) -name '*.cpp')
CPP_OBJS := $(addprefix $(CPP_OBJDIR)/,${CPP_SRCS:.cpp=.o})

C_SRCS=
C_OBJS=$(addprefix $(C_OBJDIR)/,${C_SRCS:.c=.o})

OBJS=$(CPP_OBJS) $(C_OBJS)

BIN=$(BUILDDIR)/loopbox.bin
ELF=${BIN:.bin=.elf}
MAP=$(ELF).map
LDSCRIPT=samd21.ld

DEPS=${OBJS:.o=.o.d}

.PHONY: all
all: $(BIN)

-include $(DEPS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(ELF) $(BIN) $(MAP) $(DEPS)
	find $(BUILDDIR) -name '*.o' -delete
	find $(BUILDDIR) -name '*.d' -delete

$(CPP_OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@ -MT $@ -MF $@.d

$(C_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@ -MT $@ -MF $@.d

$(ELF): $(OBJS) $(LDSCRIPT)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -T $(LDSCRIPT) -Wl,-Map=$(MAP) $(OBJS) -o $(ELF)

%.bin: %.elf
	@mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@

.PHONY: flash
flash: $(BIN) flash.jlink
	JLinkExe -device ATSAMD21G15 -if SWD -speed 4000 -autoconnect 1 -CommanderScript flash.jlink
