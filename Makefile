CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I include

SRCS := src/main.cpp \
        src/Storage/FileManager.cpp \
        src/Repository/CustomerRepository.cpp \
        src/Repository/FieldRepository.cpp \
        src/Repository/BookingRepository.cpp \
        src/Repository/TournamentRepository.cpp \
        src/Service/CustomerService.cpp \
        src/Service/FieldService.cpp \
        src/Service/BookingService.cpp \
        src/Service/TournamentService.cpp \
        src/Service/AccountService.cpp \
        src/UI/ConsoleApp.cpp

TARGET := QuanLySanBong_Console

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe