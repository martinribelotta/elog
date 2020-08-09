TARGET:=decoder

all: $(TARGET)

CFLAGS+=-Og -g3

SRC:=$(wildcard *.c)
OBJ:=$(patsubst %.c, %.o, $(SRC))
DEP:=$(patsubst %.o, %.d, $(OBJ))

%.o: %.c
	$(CC) -MMD -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ) $(DEP) $(TARGET)

-include $(DEP)
