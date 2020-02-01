NAME=fractal-nova

CFLAGS=-Wall -gstabs -O0
LDFLAGS= -athread=native -lauto

OBJS=main.o GuiWindow.o

$(NAME): $(OBJS)
	g++ -o $@ $(OBJS) $(LDFLAGS)

main.o: main.cpp
	g++ -o $@ -c $< $(CFLAGS)

GuiWindow.o: GuiWindow.cpp
	g++ -o $@ -c $< $(CFLAGS)

clean:
	delete #?.o
