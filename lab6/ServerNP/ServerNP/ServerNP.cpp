#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Windows.h"

// локайльный формат имени канала, "." - локальный пк
#define NAME L"\\\\.\\pipe\\Tube"
// в сетевом формате "." меняется на имя пк

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");

    HANDLE sH;
    DWORD lp;
    char buf[50];

    try {
        cout << "ServerNP\n\n";

        // создать именованный канал
        if ((sH = CreateNamedPipe(NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL)) == INVALID_HANDLE_VALUE) 
            throw SetPipeError("CreateNamedPipe: ", GetLastError());

        while (true) {
            cout << "Awaiting connection...\n\n";

            // соединить сервер с каналом (соединяется с CreateFile на клиенте)
            if (!ConnectNamedPipe(sH, NULL)) 
                throw SetPipeError("ConnectNamedPipe: ", GetLastError());
            
            while (true) {
                // чтение данных из канала в буфер
                if (ReadFile(sH, buf, sizeof(buf), &lp, NULL)) {
                    if (strcmp(buf, "STOP") == 0) {
                        cout << endl;
                        break;
                    }
                    cout << buf << endl;

                    // запись данных в канал
                    if (WriteFile(sH, buf, sizeof(buf), &lp, NULL)) {
                        if (strstr(buf, "ClientNPct")) break;
                    }

                    else throw SetPipeError("WriteFile: ", GetLastError());
                }

                else throw SetPipeError("ReadFile: ", GetLastError());
            }

            // закончить обмен данными
            if (!DisconnectNamedPipe(sH)) 
                throw SetPipeError("DisconnectNamedPipe: ", GetLastError());
        }

        if (!CloseHandle(sH)) 
            throw SetPipeError("CloseHandle: ", GetLastError());

        system("pause");
    }
    catch (string ErrorPipeText) {
        cout << endl << ErrorPipeText;
    }
}