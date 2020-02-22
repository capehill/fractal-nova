NAME=fractal-nova

CFLAGS=-Wall -Wextra -gstabs -O3
LDFLAGS= -athread=native -lauto

SHADERS= \
shaders/simple.vert.spv \
shaders/simple.frag.spv \
shaders/mandelbrot.vert.spv \
shaders/mandelbrot.frag.spv \
shaders/julia.vert.spv \
shaders/julia.frag.spv

OBJS=main.o GuiWindow.o NovaContext.o Timer.o Palette.o NovaObject.o Texture.o DataBuffer.o Shader.o VertexBuffer.o Program.o BackBuffer.o
DEPS = $(OBJS:.o=.d) 

$(NAME)_debug: $(OBJS) $(SHADERS)
	g++ -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	g++ -o $@ -c $< $(CFLAGS)

# Dependencies
%.d : %.cpp
	g++ -MM -MP -MT $(@:.d=.o) -o $@ $< $(CFLAGS)   

shaders/%.vert.spv: %.vert
	glslangValidator -G -o $@ $<

shaders/%.frag.spv: %.frag
	glslangValidator -G -o $@ $<

clean:
	delete $(OBJS) $(SHADERS)

strip:
	strip $(NAME)_debug -o $(NAME)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif   
