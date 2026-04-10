CXX = g++
CXXFLAGS = -Wall -Wextra -I./solution -I./googletest/googletest/include
LDFLAGS = -lgtest -lgtest_main -lpthread -pthread -Lgoogletest/build/lib

# Пути к исходным файлам
TEST_SRC = for_tests/main.cpp for_tests/test.cpp func.cpp 
TEST_EXEC = run_tests


# Запуск основного  мэйна
CXXFLAGSMAIN = -Wall -Wextra -std=c++17 #17 = версии, можно менять
TARGET = main

SRCS = main.cpp func.cpp 

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

# Скачивание и сборка googletest (выполняется один раз)
googletest:
	git clone --depth=1 --branch=release-1.11.0 https://github.com/google/googletest.git
	cd googletest && mkdir -p build && cd build && cmake .. && make

# Очистка
clear:
	rm -f $(TEST_EXEC)

# Глубокая очистка (удаляет googletest)
distclean: clean
	rm -rf googletest

.PHONY: all test clean distclean