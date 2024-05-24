// ConsoleApplication3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define WIN32_LEAN_AND_MEAN 
#include  <Windows.h>
#include <WS2tcpip.h>
#include <Winsock2.h>
#include <iostream>

using namespace std; // делаем так, чтобы можно было писать cout без ::Std
int main()
{
    SetConsoleCP(1251);           // устанавливаем русский язык                        
    SetConsoleOutputCP(1251);     // устанавливаем русский язык
    WSADATA wsaData;              // харинт в себе информацию о том, как взаимодействовать с сокетами            
    ADDRINFO hints;               //структура, содержащая в себе информацию о сокете ( какой протокол, семейства и тд)
    ADDRINFO* addrResult;         //структура, содержащая в себе информацию о сокете ( какой протокол, семейства и тд)
    SOCKET ConnectSocket = INVALID_SOCKET; // при создании сокета даётся значение INVALID_SOCKET. SOCKET, в сути, это ENUM, хранящий значения 1,2,3,4,5, где каждое имеет свой функцилнал. INVALID_SOCEKT - это -1, сделано для того, чтобы не было магического числа
    const char* sendBuffer = "\nHello from Client"; // сообщение, выводимое на сервере

    char recvBuffer[512]; //создание буффера, в котором может хранится сообщение до 512б.

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  //функция, в которую передаётся версия WinStock (WinStock - это API, с ифнормацией о том, как Windows должен работать с сокетом), и переменную, в которой должны хранится инструкции. При получении информации из первой переменной, он запишет их во вторую
    if (result != 0) {
        cout << "беда" << endl; // выбрасывает ошибку, если result != 0
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуляет память, принимает в себя 2 параметра, где первый - это то, с какого момента чистить память, второй - на какое количество байт мы обнуляем
    hints.ai_family = AF_INET;         // указание семейства адресов IPv4
    hints.ai_socktype = SOCK_STREAM;   // используется для указания потока
    hints.ai_protocol = IPPROTO_TCP;   // указание протокола TCP.TCP - протокол, позволяющий двум пользователям сети обмениваться данными



    result = getaddrinfo("localhost", "666", &hints, &addrResult); // принимает в себя 4 параметра, первый - это название, второй - порт, третий - набор инструкций, четвёртый - это результат вывода 
    if (result != 0) {
        cout << "беда 2" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // создание сокета. Данная функция принимает 3 параметра: первый - указатель на семейство протоколов, второй - тип сокета, третий - протокол
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Сокет не создан" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    result = connect(ConnectSocket, addrResult->ai_addr, addrResult->ai_addrlen); // устанавлиеваем соединение  с сервером. Функция connect принимает 3 параметра: первый - сам сокет, второй  - адресс сервера, третий -   длинна адреса .
    if (result == SOCKET_ERROR) {
        cout << "нет, сервера нема" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        return 1;
    }

    result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0); // отправка сообщения. Принимает 4 параметра: первый - сокет, второй - буффер, третий -длинна в байтах, четвёртый - флаг, определяющий способ выполнения вызова.
    if (result == SOCKET_ERROR) {
        cout << "send failde error" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "send" << result << endl;

    result = shutdown(ConnectSocket, SD_SEND); // закрывет  возможность отправлять сообщения через сокет. Принимает 2 параметра, где первый - это сам сокет, второй - это SD_SEND, из enum'а, определяющий то, что блокирует ( в данном случае блокирует отправку)
    if (result == SOCKET_ERROR) {
        cout << "shutdown failde error" << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    do {
        ZeroMemory(recvBuffer, 512, 0);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "received" << result << "bytes" << endl;
            cout << "received data" << recvBuffer<< endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;
        }
        else {
            cout << "received with ERRROR" << endl;
        }

    } while (result > 0);
    freeaddrinfo(addrResult); // освобождение памяти
    WSACleanup();
    return 1;
}

