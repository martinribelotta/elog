TARGET=elog-test

DEFINES:=
INCLUDES:=inc
SRCDIR=src
SPECS=nosys nano
VERBOSE=n
OPTIMIZE=g

DEST=out

ARCH:=-mcpu=cortex-m3 -mthumb
LDSCRIPTS:=armv7m-generic.ld
LIBPATH:=lib
LIB:=c m

ELOG_HEADER:=inc/elog-internal.h

################################################################

ELF=$(addsuffix .elf, $(addprefix $(DEST)/, $(TARGET)))
MAP=$(patsubst %.elf, %.map, $(ELF))
BIN=$(patsubst %.elf, %.bin, $(ELF))
HEX=$(patsubst %.elf, %.hex, $(ELF))
LST=$(patsubst %.elf, %.lst, $(ELF))
NMP=$(patsubst %.elf, %.nmp, $(ELF))
BINLOG=$(DEST)/log_text.bin

CFLAGS:=$(ARCH)
CFLAGS+=$(addprefix -I, $(INCLUDES))
CFLAGS+=$(addprefix -D, $(DEFINES))
CFLAGS+=-ffunction-sections -fdata-sections -nostartfiles
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
LDFLAGS+=-nostartfiles

LDSCRIPT_FILES:=$(foreach l, $(LDSCRIPTS), $(foreach L, $(LIBPATH), $(wildcard $(L)/$(l))))

LSTSECTIONS:=$(foreach l, $(LDSCRIPT_FILES), $(strip $(shell cat $l | grep -E '\s\.\S+\s+\:' | cut -f 1 -d ':') ))
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

all: $(ELF) $(LST) $(NMP) $(BINLOG) $(BIN) $(HEX) build-decoder

OBJS:=$(addprefix $(DEST)/, $(patsubst %.c, %.o, $(notdir $(CSRC))))
DEPS:=$(addsuffix %.dep.mk, $(OBJS))

$(DEST):
	$(Q)$(MD) $@

define cc_rule

$(DEST)/$(notdir $(patsubst %.c, %.o, $1)): $1 | $(DEST)
	@echo CC $$<
	$(Q)$(CC) -MMD -MP -MF $$@.dep.mk -c $(CFLAGS) -o $$@ $$<

endef

define cc_rules
	$(foreach f, $(CSRC), $(eval $(call cc_rule, $f)))
endef

$(eval $(cc_rules))

$(ELF): $(OBJ)
	@echo LD $@
	$(Q)$(LD) -o $@ $(LDFLAGS) $(OBJS)

$(BINLOG): $(ELF)
	@echo EXTRACT LOGS $@
	$(Q)$(OCP) --dump-section .elog=$@ $<
	@echo LOGS NOLOAD ELF $<
	@$(OCP) --set-section-flags .elog=noload $<

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
	$(Q)$(MAKE) -C decoder clean

%.d:
%.h:
-include $(DEPS)

build-decoder:
	$(Q)$(MAKE) -C decoder ELOG_HEADER=$(dir $(realpath $(ELOG_HEADER)))

.PHONY: all clean build-decoder
.DEFAULT: all
