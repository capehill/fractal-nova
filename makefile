NAME = FractalNova

COMPILER = g++
CFLAGS = -Wall -Wextra -Wpedantic -Wconversion -Werror -gstabs -O3 -std=c++17
LDFLAGS = -athread=single -lauto

SHADERS = shaders/mandelbrot.vert.spv \
          shaders/mandelbrot.frag.spv \
          shaders/julia.vert.spv \
          shaders/julia.frag.spv

SRCS = $(wildcard *.cpp)
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
shaders/%.vert.spv: %.vert
	glslangValidator -G -o $@ $<

shaders/%.frag.spv: %.frag
	glslangValidator -G -o $@ $<

clean:
	delete $(OBJS) $(SHADERS)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif   
