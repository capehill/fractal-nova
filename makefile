NAME=fractal-nova

CFLAGS=-Wall -gstabs -O3
LDFLAGS= -athread=native -lauto

SHADERS= \
shaders/simple.vert.spv \
shaders/simple.frag.spv \
shaders/mandelbrot.vert.spv \
shaders/mandelbrot.frag.spv

OBJS=main.o GuiWindow.o NovaContext.o Timer.o

$(NAME): $(OBJS) $(SHADERS)
	g++ -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	g++ -o $@ -c $< $(CFLAGS)

shaders/%.vert.spv: %.vert
	glslangValidator -G -o $@ $<

shaders/%.frag.spv: %.frag
	glslangValidator -G -o $@ $<

clean:
	delete $(OBJS) $(SHADERS)
