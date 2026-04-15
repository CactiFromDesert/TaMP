CXX = g++
CXX = g++
CXXFLAGS = -Wall -Wextra -I./solution -I./googletest/googletest/include -fPIC
LDFLAGS = -lgtest -lgtest_main -lpthread -pthread -Lgoogletest/build/lib

# Пути к исходным файлам
TEST_SRC = for_tests/main.cpp for_tests/test.cpp TCPClient.cpp 
TEST_EXEC = run_tests

# Запуск основного мэйна
CXXFLAGSMAIN = -Wall -Wextra -std=c++17 #17 = версии, можно менять
TARGET = main

SRCS = main_client.cpp TCPClient.cpp 

# Для динамической библиотеки
LIB_SRC = func.cpp
LIB_OBJECTS = $(LIB_SRC:.cpp=.o)
LIB_TARGET = libfunc.so

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGSMAIN) $(SRCS) -o $(TARGET)

vallg: main
	valgrind ./$(TARGET)

# Правило по умолчанию (собирает и запускает тесты)
# all: $(TEST_EXEC)
#	./$(TEST_EXEC)

# Сборка тестового исполняемого файла
$(TEST_EXEC): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Запуск тестов (можно использовать вместо all)
test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Сборка динамической библиотеки
$(LIB_TARGET): $(LIB_OBJECTS)
	$(CXX) -shared -o $@ $^
	@echo "Динамическая библиотека $(LIB_TARGET) создана"

# Компиляция объектных файлов для библиотеки (с -fPIC)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Сборка и использование библиотеки с main_lib.cpp (пример)
LIB_EXAMPLE_SRC = main_lib.cpp
LIB_EXAMPLE_TARGET = app_with_lib

$(LIB_EXAMPLE_TARGET): $(LIB_EXAMPLE_SRC) $(LIB_TARGET)
	$(CXX) $(CXXFLAGSMAIN) $< -L. -lfunc -o $@
	@echo "Исполняемый файл с библиотекой создан"
	@echo "Для запуска: LD_LIBRARY_PATH=. ./$(LIB_EXAMPLE_TARGET)"

# Скачивание и сборка googletest (выполняется один раз)
googletest:
	git clone --depth=1 --branch=release-1.11.0 https://github.com/google/googletest.git
	cd googletest && mkdir -p build && cd build && cmake .. && make

# Очистка
clean:
	rm -f $(TEST_EXEC) $(TARGET) $(LIB_OBJECTS) $(LIB_TARGET) $(LIB_EXAMPLE_TARGET)

# Глубокая очистка (удаляет googletest)
distclean: clean
	rm -rf googletest

# Запуск примера с библиотекой
run_lib: $(LIB_EXAMPLE_TARGET)
	LD_LIBRARY_PATH=. ./$(LIB_EXAMPLE_TARGET)

.PHONY: all test clean distclean run_lib