TARGET=elog-test

DEFINES:=
INCLUDES:=inc
SRCDIR=src
SPECS=nosys
VERBOSE=n
OPTIMIZE=g

DEST=out

ARCH:=-mcpu=cortex-m3 -mthumb
LDSCRIPTS:=armv7m-generic.ld
LIBPATH:=lib

################################################################

ELF=$(addsuffix .elf, $(addprefix $(DEST)/, $(TARGET)))
MAP=$(patsubst %.elf, %.map, $(ELF))
BIN=$(patsubst %.elf, %.bin, $(ELF))
HEX=$(patsubst %.elf, %.hex, $(ELF))
LST=$(patsubst %.elf, %.lst, $(ELF))
NMP=$(patsubst %.elf, %.nmp, $(ELF))

CFLAGS:=$(ARCH)
CFLAGS+=$(addprefix -I, $(INCLUDES))
CFLAGS+=$(addprefix -D, $(DEFINES))
CFLAGS+=-ffunction-sections -fdata-sections
CFLAGS+=-O$(OPTIMIZE)
CFLAGS+=-g3

LD_FLAGS:=--no-print-map-discarded
LD_FLAGS+=-Map=$(strip $(MAP))
LD_FLAGS+=--gc-sections
LD_FLAGS+=--cref
LD_FLAGS+=--print-memory-usage

COMMA:=,

LDFLAGS:=$(ARCH)
LDFLAGS+=$(addprefix -L, $(LIBPATH))
LDFLAGS+=$(addprefix -l, $(LIBS))
LDFLAGS+=$(addprefix -T, $(LDSCRIPTS))
LDFLAGS+=$(addsuffix .specs, $(addprefix -specs=, $(SPECS)))
LDFLAGS+=$(addprefix -Wl$(COMMA), $(LD_FLAGS))

LSTSECTIONS:=.text .rodata .data .bss .isr_vectors
LSTFLAGS:=-z -x -w -t -S $(addprefix -j, $(LSTSECTIONS))

NMFLAGS:=-C -f sysv -S --size-sort -t dec

CSRC=$(foreach d, $(SRCDIR), $(wildcard $(d)/*.c))
OBJ=$(patsubst %.c, $(DEST)/%.o, $(notdir $(CSRC)))

ifeq ($(VERBOSE),y)
Q:=
else
Q:=@
endif

CROSS=arm-none-eabi-
CC=$(CROSS)gcc
LD=$(CROSS)gcc
OD=$(CROSS)objdump
OCP=$(CROSS)objcopy
SZ=$(CROSS)size
NM=$(CROSS)nm
RM=rm -fr
MD=mkdir -p

all: $(ELF) $(BIN) $(HEX) $(LST) $(NMP)

OBJS:=$(addprefix $(DEST)/, $(patsubst %.c, %.o, $(notdir $(CSRC))))

$(DEST):
	$(Q)$(MD) $@

define cc_rule

$(DEST)/$(notdir $(patsubst %.c, %.o, $1)): $1 | $(DEST)
	@echo CC $$<
	$(Q)$(CC) -c $(CFLAGS) -o $$@ $$<

endef

define cc_rules
	$(foreach f, $(CSRC), $(eval $(call cc_rule, $f)))
endef

$(eval $(cc_rules))

$(ELF): $(OBJ)
	@echo LD $@
	$(Q)$(LD) -o $@ $(LDFLAGS) $(OBJS)

$(BIN): $(ELF)
	@echo BIN $<
	$(Q)$(OCP) -O binary $< $@

$(HEX): $(ELF)
	@echo HEX $<
	$(Q)$(OCP) -O ihex $< $@

$(LST): $(ELF)
	@echo LIST $<
	$(Q)$(OD) $(LSTFLAGS) $< > $@

$(NMP): $(ELF)
	@echo NM $<
	$(Q)$(NM) $(NMFLAGS) $< > $@

clean:
	@echo CLEAN
	$(Q)$(RM) $(DEST)

.PHONY: all clean
.DEFAULT: all