C = gcc
RM = rm -f

CFLAGS = -Wall -g -lpthread

ECHO = echo

BIN = shell
OBJS = Bank.o appserver.o Queue.o

all: $(BIN)


$(BIN): $(OBJS)
			@$(ECHO) Linking $@
					@$(CC) $^ -o  $@

-include $(OBJS:.o=.d)

%.o: %.c
			@$(ECHO) Compiling $<
					@$(CC) -MMD -MF $*.d -c $<

.PHONY: all clean

clean:
		@$(RM) *.o $(BIN) *.d core vgcore.* gmon.out

clobber: clean
