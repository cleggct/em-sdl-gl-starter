This code creates an SDL window with an OpenGL rendering context and can be cross-compiled to a web target via 
emscripten. There is also some basic stuff for taking keyboard input, game states, and other things. Intended 
as a starting point for developing graphics applications for the web.

`sh
make       # native build
make web   # web build
make serve # run dev server
`
