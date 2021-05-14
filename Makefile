# the platform of the build
# currently supported values are:
# - linux
PLATFORM = linux
# currently supported values are:
# - x64
ARCH = x64

# base engine source directories
SRCS_DIRS = source
# base engine source files
define SRCS
source/C3D.c
source/ColorMap.c
source/Controller.c
source/CursesDevice.c
source/GameLoop.c
source/Geometry.c
source/ObjLoader.c
source/Rasterizer.c
source/Splash.c
source/VecMath.c
source/VecTypes.c
endef

# base engine header files
define DEPS
source/ColorMap.h
source/Controller.h
source/Device.h
source/GameLoop.h
source/Geometry.h
source/ObjLoader.h
source/Rasterizer.h
source/RenderTypes.h
source/Splash.h
source/VecMath.h
source/VecTypes.h
endef

OBJS_BASE_DIR = ./build/$(PLATFORM)/$(ARCH)/objs
OBJS = $(patsubst %.c,$(OBJS_BASE_DIR)/%.o,$(SRCS))

TARGET_DIR = ./build/$(PLATFORM)/$(ARCH)
TARGET = c3d

# platform and architecture compiler, options, and extra sources and headers
ifeq ($(PLATFORM),linux)
# use GCC as the compiler on linux
CC = gcc

CFLAGS = --std=c11 -Wall -g

SRCFLAG = -c
OUTFLAG = -o

INCLUDES = -I./source

LFLAGS = 
LIBS = -lm -lncurses

ifeq ($(PLATFORM),x64)
# enable AVX support on x64 systems
CFLAGS += -mavx
endif
endif

.PHONY: all clean
.SECONDARY: main-build

all: pre-build main-build

pre-build:
	@echo "Build starting for $(PLATFORM) $(ARCH)."

post-build:
	@echo "Build finished."

main-build: $(TARGET_DIR)/$(TARGET)

# $$(OBJS_BASE_DIR)/$(1)/%.o: $(1)/%.c $$(DEPS)
# @mkdir -p $$(OBJS_BASE_DIR)/$(1)
# $$(CC) $$(CFLAGS) $$(SRCFLAG) $$(OUTFLAG) $@ $< $$(INCLUDES)
define GenerateObjRules
$(OBJS_BASE_DIR)/$(1)/%.o: $(1)/%.c $(DEPS)
endef

$(foreach src_dir,$(SRCS_DIRS),$(eval $(call GenerateObjRules,$(src_dir))))

$(TARGET_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) $(OUTFLAG) $@ $^ $(LFLAGS) $(LIBS)

clean:
	rm -f $(OBJS)
