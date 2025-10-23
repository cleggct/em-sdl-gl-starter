# Simple cross-build: native (gcc) and web (Emscripten)
# Debian deps (native):  sudo apt-get install libsdl2-dev libsdl2-image-dev
# Emscripten:            https://emscripten.org/docs/getting_started/downloads.html

APP      := emstarter
SRC_DIR  := src
OBJ_DIR  := build

# ---- Sources / Objects ----
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# ---- Native (gcc/clang) ----
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 \
           $(shell sdl2-config --cflags) \
           $(shell pkg-config --cflags SDL2_image)
LDLIBS  := $(shell sdl2-config --libs) \
           $(shell pkg-config --libs SDL2_image) \
           -lGLESv2

# ---- Web (Emscripten) ----
EMCC    := emcc
# Core flags
EMCFLAGS := -O2 -sWASM=1 -sALLOW_MEMORY_GROWTH=1 \
            -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png"]' \
            -sMIN_WEBGL_VERSION=1 -sMAX_WEBGL_VERSION=2
# SDL2 + SDL_image from Emscripten ports
EMCFLAGS += -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png"]'
# WebGL settings (we use SDL2 renderer; these are harmless hints)
EMLDFLAGS := $(EMCFLAGS) -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
# Preload your assets folder to /assets in the virtual FS
PRELOAD  := --preload-file assets@/assets
WEB_OUT  := web/index.html

.PHONY: all run clean web serve native

# Default: native
all: $(APP)

$(APP): $(OBJS)
	@echo "  LINK  $@"
	@$(CC) $(OBJS) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

run: $(APP)
	./$(APP)

# ---- Web target using emcc directly (no object reuse to keep it simple) ----
web:
	@echo "  EMCC  $(WEB_OUT)"
	@$(EMCC) $(SRCS) $(EMLDFLAGS) $(PRELOAD) -o $(WEB_OUT)

# Simple dev server to test the web build
serve: web
	@echo "Serving ./web at http://localhost:8000/ ..."
	@cd web && python3 -m http.server 8000

clean:
	rm -rf $(OBJ_DIR) $(APP) web/index.html web/index.js web/index.wasm web/*.wasm web/*.js

