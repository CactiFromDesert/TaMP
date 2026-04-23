CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread
LDFLAGS = -pthread

# Для сервера
SERVER_SRC = main_server.cpp TCPServer.cpp
SERVER_TARGET = server

# Для клиента
CLIENT_SRC = main_client.cpp TCPClient.cpp
CLIENT_TARGET = client

# Сборка сервера
build_server: $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o $(SERVER_TARGET)
	@echo "✅ Сервер успешно собран: ./$(SERVER_TARGET)"

# Сборка клиента
build_client: $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $(CLIENT_TARGET)
	@echo "✅ Клиент успешно собран: ./$(CLIENT_TARGET)"

# Запуск сервера
server: build_server
	@echo "🚀 Запуск сервера..."
	./$(SERVER_TARGET)

# Запуск клиента
client: build_client
	@echo "🚀 Запуск клиента..."
	./$(CLIENT_TARGET)

# Очистка
clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "🧹 Очистка завершена"

# Сборка всего
all: build_server build_client
	@echo "📦 Все компоненты собраны"

.PHONY: all clean server client build_server build_client