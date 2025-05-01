# Fractal Nova

Real-time Mandelbrot / Julia fractal, calculated by the GPU.

## Requirements:

Warp3D Nova library version 54.

## Controls

- Use mouse to pan display using the LMB.
- Use RMB to select a fractal and its palette from menu.
- Use mouse wheel (or CURSOR UP/DOWN) to zoom in and out.
- Hold SHIFT key to zoom faster.
- Press SPACE bar to reset display.
- Press ESC key to quit.

## Icon tooltypes

VSYNC: limit the drawing speed.
FULLSCREEN: start with fullscreen mode.
LAZYCLEAR: don't clear backbuffer every frame.
ITERATIONS: iteration depth. Range 100-1000.
LOGLEVEL: for debugging.
SCREENMODE: preferred fullscreen mode.
WINDOWSIZE: preferred window size.

## Version 1.1 changes

- Add fullscreen mode
- Add icon tooltypes
- Add about requester
- Add iconification
- Add Control-C handling
- Remove command-line parameters
- Start using window.class
- Improve RMB menus
- Improve colour mapping
- Improve logger
- Improve some log messages
- Fix texture sampler resource leak
- Fix texture coordinate setup
- Fix some compiler warnings
- Fix some CppCheck issues
- Refactor code


