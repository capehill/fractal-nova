NAME=fractal-nova

CFLAGS=-Wall -gstabs -O0
LDFLAGS= -athread=native -lauto

OBJS=main.o GuiWindow.o NovaContext.o

$(NAME): $(OBJS)
	g++ -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	g++ -o $@ -c $< $(CFLAGS)

#main.o: main.cpp 
#	g++ -o $@ -c $< $(CFLAGS)

#GuiWindow.o: GuiWindow.cpp
#	g++ -o $@ -c $< $(CFLAGS)

#NovaContext.o: NovaContext.cpp

clean:
	delete #?.o
