NAME=fractal-nova

CFLAGS=-Wall -gstabs -O0
LDFLAGS= -athread=native -lauto

SHADERS=simple.vert.spv simple.frag.spv
OBJS=main.o GuiWindow.o NovaContext.o

$(NAME): $(OBJS) $(SHADERS)
	g++ -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	g++ -o $@ -c $< $(CFLAGS)

%.vert.spv: %.vert
	glslangValidator -G -o $@ $<

%.frag.spv: %.frag
	glslangValidator -G -o $@ $<

#main.o: main.cpp 
#	g++ -o $@ -c $< $(CFLAGS)

#GuiWindow.o: GuiWindow.cpp
#	g++ -o $@ -c $< $(CFLAGS)

#NovaContext.o: NovaContext.cpp

clean:
	delete $(OBJS) $(SHADERS)
