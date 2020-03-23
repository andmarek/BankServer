C = gcc
RM = rm -f

CFLAGS = -Wall -g

ECHO = echo

LDFLAGS = -pthread

BIN = bankserver
OBJS = Bank.o appserver.o queue.o

all: $(BIN)


$(BIN): $(OBJS)
			@$(ECHO) Linking $@
					@$(CC) $^ -o  $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
			@$(ECHO) Compiling $<
					@$(CC) -MMD -MF $*.d -c $<

.PHONY: all clean

clean:
		@$(RM) *.o $(BIN) *.d core vgcore.* gmon.out

clobber: clean
