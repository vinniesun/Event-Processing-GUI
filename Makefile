#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

CXX = g++
#CXX = clang++

EXE = event_gui

IMGUI_DIR = imgui
EVENT_DIR = events
OFILE_DIR = ofile

SOURCES = main.cpp
SOURCES += $(IMGUI_DIR)/src/imgui.cpp $(IMGUI_DIR)/src/imgui_demo.cpp $(IMGUI_DIR)/src/imgui_draw.cpp $(IMGUI_DIR)/src/imgui_tables.cpp $(IMGUI_DIR)/src/imgui_widgets.cpp $(IMGUI_DIR)/src/imgui_stdlib.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
SOURCES += $(EVENT_DIR)/src/guiprocess.cpp
SOURCES += $(EVENT_DIR)/src/arcstar.cpp $(EVENT_DIR)/src/bloomfilter.cpp $(EVENT_DIR)/src/customharris.cpp $(EVENT_DIR)/src/efast.cpp $(EVENT_DIR)/src/luvharris.cpp
SOURCES += $(EVENT_DIR)/src/process.cpp $(EVENT_DIR)/src/sfast.cpp 

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CXXFLAGS = -std=c++11 -I$(IMGUI_DIR)/include -I$(IMGUI_DIR)/backends -I$(EVENT_DIR)/include -mmacosx-version-min=12.2
# CXXFLAGS for Intel Mac
#CXXFLAGS += -I/usr/local/Cellar/eigen/3.4.0_1/include/eigen3/ -I/usr/local/Cellar/opencv/4.5.5_1/include/opencv4/
# CXXFLAGS for M1 Mac
CXXFLAGS += -I/opt/homebrew/Cellar/eigen/3.4.0_1/include/eigen3/ -I/usr/local/include/opencv4/
CXXFLAGS += -g -Wall -Wformat -O3
LIBS =

##---------------------------------------------------------------------
## OPENGL ES
##---------------------------------------------------------------------

## This assumes a GL ES library available in the system, e.g. libGLESv2.so
# CXXFLAGS += -DIMGUI_IMPL_OPENGL_ES2
# LINUX_GL_LIBS = -lGLESv2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) `pkg-config --static --libs glfw3`

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib
	LIBS += -lglfw3
	#LIBS += -lglfw
	LDFLAGS = -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_videoio -lopencv_photo 

	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
# .PHONY: create_build_dir
# create_build_dir:
# 	mkdir -p $(OFILE_DIR)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(EVENT_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) $(LDFLAGS)

clean:
	rm -f $(EXE) $(OBJS)
