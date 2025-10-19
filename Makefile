# Edit if needed
PROJECT_NAME := cshmup
SRC_DIR := src
BUILD   := build
TARGET  := index

# Sources
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD)/%.o,$(SRCS))

# Emscripten
EMCC := emcc

# Core flags
CFLAGS  := -O2 -sWASM=1 -sALLOW_MEMORY_GROWTH=1
# Use SDL2 from emscripten ports:
CFLAGS  += -sUSE_SDL=2
# (optional) SDL_image + PNG support:
# CFLAGS += -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png"]'

# Output format: html (or .js/.wasm pair if you prefer)
LDFLAGS := $(CFLAGS) -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
# Preload assets folder to virtual FS at /assets
PRELOAD := --preload-file assets@/assets

# Phony targets
.PHONY: all web clean serve native

all: web

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
	$(EMCC) $(CFLAGS) -c $< -o $@

web: $(OBJS)
	$(EMCC) $^ $(LDFLAGS) $(PRELOAD) -o $(TARGET).html
	@echo "Built $(TARGET).html (open in a server, not file://)"

clean:
	rm -rf $(BUILD) $(TARGET).html $(TARGET).wasm $(TARGET).js

# Quick dev server (requires python3)
serve: web
	python3 -m http.server 8000

# Optional native build (no emscripten) to test locally
native:
	$(CC) -O2 $(SRCS) `sdl2-config --cflags --libs` -o $(PROJECT_NAME)

