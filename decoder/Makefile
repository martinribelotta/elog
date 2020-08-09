TARGET:=elog_decoder

all: $(TARGET)

Q:=@

CC=gcc
LD=gcc

CFLAGS+=-Og -g3 -I$(ELOG_HEADER)

# Disable -Waddress-of-packed-member false positive warning.
# See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88664
# And: https://patchwork.kernel.org/patch/11265041/
CFLAGS+=-Wno-address-of-packed-member

SRC:=$(wildcard *.c)
OBJ:=$(patsubst %.c, %.o, $(SRC))
DEP:=$(patsubst %.o, %.d, $(OBJ))

%.o: %.c
	@echo CC $<
	$(Q)$(CC) -MMD -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJ)
	@echo LD $@
	$(Q)$(LD) -o $@ $(OBJ) $(LDFLAGS)

clean:
	@echo CLEAN
	$(Q)$(RM) $(OBJ) $(DEP) $(TARGET)

-include $(DEP)
