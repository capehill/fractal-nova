NAME = FractalNova

COMPILER = ppc-amigaos-g++
CFLAGS = -Wall -Wextra -Wpedantic -Wconversion -Werror -gstabs -O3 -std=c++17
LDFLAGS = -athread=single -lauto

SHADERS = shaders/mandelbrot.vert.spv \
          shaders/mandelbrot.frag.spv \
          shaders/julia.vert.spv \
          shaders/julia.frag.spv

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

DEPS = $(OBJS:.o=.d)

$(NAME): $(NAME)_debug
	strip $(NAME)_debug -o $(NAME)

$(NAME)_debug: $(OBJS) $(SHADERS)
	$(COMPILER) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(COMPILER) -o $@ -c $< $(CFLAGS)

# Dependencies
%.d : %.cpp
	$(COMPILER) -MM -MP -MT $(@:.d=.o) -o $@ $< $(CFLAGS)

# Shaders
shaders/%.vert.spv: glsl/%.vert
	glslangValidator -G -o $@ $<

shaders/%.frag.spv: glsl/%.frag
	glslangValidator -G -o $@ $<

clean:
	rm $(OBJS) $(DEPS) $(SHADERS)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
