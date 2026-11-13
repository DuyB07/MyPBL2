CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Iinclude

BUILD_DIR      := build
CONSOLE_TARGET := QuanLySanBong_Console
GUI_TARGET     := $(BUILD_DIR)/QuanLySanBong_GUI

CORE_BACKEND_SRCS := src/Storage/FileManager.cpp \
                     src/Repository/CustomerRepository.cpp \
                     src/Repository/FieldRepository.cpp \
                     src/Repository/BookingRepository.cpp \
                     src/Repository/TournamentRepository.cpp \
                     src/Service/CustomerService.cpp \
                     src/Service/FieldService.cpp \
                     src/Service/BookingService.cpp \
                     src/Service/TournamentService.cpp \
                     src/Service/AccountService.cpp

GUI_MODULE_DIRS := src/GUI/Core \
                   src/GUI/Theme \
                   src/GUI/Components \
                   src/GUI/Screens/Auth \
                   src/GUI/Screens/Owner \
                   src/GUI/Screens/Customer

GUI_MODULE_SRCS := $(foreach dir,$(GUI_MODULE_DIRS),$(wildcard $(dir)/*.cpp))

CONSOLE_SRCS := src/main.cpp src/UI/ConsoleApp.cpp $(CORE_BACKEND_SRCS)
GUI_SRCS     := src/main_gui.cpp $(GUI_MODULE_SRCS) $(CORE_BACKEND_SRCS)

IMGUI_SOURCES := $(wildcard third_party/imgui/*.cpp) \
                 third_party/imgui/backends/imgui_impl_glfw.cpp \
                 third_party/imgui/backends/imgui_impl_opengl3.cpp

ifeq ($(OS),Windows_NT)
    CONSOLE_TARGET := $(CONSOLE_TARGET).exe
    GUI_TARGET     := $(GUI_TARGET).exe
    GLFW_INCLUDE   := -Ithird_party/glfw/include
    GLFW_LIB       := -Lthird_party/glfw/lib -lglfw3 -lopengl32 -lgdi32 -static-libgcc -static-libstdc++
else
    GLFW_INCLUDE   :=
    GLFW_LIB       := -lglfw -lGL -ldl -lpthread
endif

GUI_CXXFLAGS := $(CXXFLAGS) -Ithird_party/imgui -Ithird_party/imgui/backends -Ithird_party/stb $(GLFW_INCLUDE)

all: gui console

gui:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(GUI_CXXFLAGS) $(GUI_SRCS) $(IMGUI_SOURCES) $(GLFW_LIB) -o $(GUI_TARGET)

console:
	$(CXX) $(CXXFLAGS) $(CONSOLE_SRCS) -o $(CONSOLE_TARGET)

run-gui: gui
	./$(GUI_TARGET)

run-console: console
	./$(CONSOLE_TARGET)

clean:
	rm -f $(GUI_TARGET) $(CONSOLE_TARGET)

.PHONY: all gui console run-gui run-console clean