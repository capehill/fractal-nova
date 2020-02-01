NAME=fractal-nova

CFLAGS=-Wall -gstabs -O0
LDFLAGS=

OBJS=main.o

$(NAME): $(OBJS)
	g++ -o $@ $(OBJS) $(LDFLAGS)

main.o: main.cpp
	g++ -o $@ -c $< $(CFLAGS)

clean:
	delete #?.o
