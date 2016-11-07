CC=gcc
PROG = snake

SRCS= snake.c
LIBS= -lncurses

all:$(PROG) 

$(PROG): $(SCRS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
