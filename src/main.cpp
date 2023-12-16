// 1. Необходимо написать программу (HTTP-клиент) на языке C++ под ОС Linux
// 2. При написании программы разрешается использовать только Posix, STL
// 3. Программа должна получать данные посредством HTTP запроса через socket от любого сервера сети Интернет
// 4. В программе необходимо использовать два потока
// 5. Основной (первый) поток должен осуществлять: постоянный контроль за наличием считанных из сокета данных; вывод считанных данных в консоль или в окно; удаление выведенных данных из контейнера std::vector
// 6. Второй поток должен осуществлять бесконечную периодическую отправку HTTP запросов посредством сокета на web-сервер и получение результата HTTP запроса из сокета, с максимально возможной скоростью
// 7. Результат HTTP запроса должен помещается в std::vector только после полного считывания ответа на HTTP запрос, при этом, элементом контейнера std::vector должен являться массив байт данных считанных из сокета – результат HTTP запроса
// 8. Программа должна продолжать выполнение до нажатия ESC, после чего второй поток должен корректно завершиться
// 9. Программа не должна допускать переполнения памяти, т.е. необходимо удалять элементы контейнера std::vector после их вывода на экран

#include "HttpClient.h"
#include <termios.h>
#include <fcntl.h>
#include <atomic>
#include <pthread.h>
#include <unistd.h>
#include<memory.h>

std::atomic<bool> running(true);

void setNonBlockingTerminalMode() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void setNormalTerminalMode() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Структура для передачи данных в поток
struct ThreadData {
    std::unique_ptr<HttpClient> client;
};

// Поток для отправки запросов
void* sendRequests(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    while (running) {
        data->client->sendRequest();
    }
    return nullptr;
}

// Поток  для обработки данных
void* processData(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    while (running) {
        data->client->printData();
        data->client->clearData();
    }
    return nullptr;
}

void CheckESC()
{
    char ch;
    while (running) {
        if (read(STDIN_FILENO, &ch, 1) > 0 && ch == 27) { // ESC
            running = false;
        }
    }
}

int main() {
    auto client = std::make_unique<HttpClient>("example.com", 80); 
    ThreadData data = { std::move(client) };

    pthread_t requestThread, processThread;
    pthread_create(&requestThread, nullptr, sendRequests, &data);
    pthread_create(&processThread, nullptr, processData, &data);

    setNonBlockingTerminalMode();
    CheckESC();
    setNormalTerminalMode();

    pthread_join(requestThread, nullptr);
    pthread_join(processThread, nullptr);

    return 0;
}
