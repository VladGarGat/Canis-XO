#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <ctime>
#pragma warning(disable: 4996)

char board[20] = {};

char a_login[256];
char a_passw[256];
bool a_flag = false;

void print_board(char*) {
    printf("Game board\n");
    printf(" %c | %c | %c \n", board[0], board[1], board[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[3], board[4], board[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[6], board[7], board[8]);
}

int get_move(void) {
    printf("\nMenu of the moves:\n");
    printf(" 1 | 2 | 3 \n");
    printf("---+---+---\n");
    printf(" 4 | 5 | 6 \n");
    printf("---+---+---\n");
    printf(" 7 | 8 | 9 \n");
    printf("\n");
    print_board((char*)":");
    printf("\nEnter your move: ");
    int move;
    scanf("%d", &move);
    while (move > 9 || move < 1 || board[move - 1] != '-') {
        printf("Enter numder from 1 to 9:\n");
        scanf("%d", &move);
    }
    return move;
}

bool has_wonQ(char player) {
    int wins[][3] = { {0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6} };
    for (int i = 0; i < 8; i++) {
        int count = 0;
        for (int j = 0; j < 3; j++)
            if (board[wins[i][j]] == player) {
                count++;
                if (count == 3)
                    return true;
            }
    }
    return false;
}

bool FoundUserData(std::ifstream& f1)
{
    f1.clear(); f1.seekg(0);
    std::string flag;
    while (f1.peek() != EOF) {
        std::getline(f1, flag);
        if (flag == "%AUTO_LOGGING_IN%") {
            std::getline(f1, flag);
            strcpy(a_login, flag.c_str());
            std::getline(f1, flag);
            strcpy(a_passw, flag.c_str());

            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {

    // В командую строку передается любое кол-во аргументов.
    // Название файла конфигурации может быть подано в виде:
    // -cfg conf.txt
    // обязательно: -cfg   название любое: conf.txt

    std::ifstream f1;
    std::string cfgname = "config.txt";
    if (argc > 2) {
        std::string str;
        for (int i = 1; i < argc - 1; i += 2) {
            str = argv[i];
            if (str == "-cfg" && ((i + 1) < argc)) {
                cfgname = argv[i + 1];
                break;
            }
        }
    }
    f1.open(cfgname);

    // читаем название лог-файла, которое должно быть первой строкой в конфиге
    std::ofstream f;
    std::string logname;
    std::getline(f1, logname);
    f.open(logname);

    // читаем две строки: адрес и порт соответственно следующими строчками из конфига
    std::string s_ip; std::string s_port;
    std::getline(f1, s_ip);
    std::getline(f1, s_port);
    const char* ip = s_ip.c_str();
    int port = stoi(s_port);


    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error" << std::endl;
        exit(1);
    }
    time_t now = time(0);
    char* dt = ctime(&now);
    f << ((std::string)dt + " - Sockets start up!") << std::endl << std::endl;
    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
    now = time(0); dt = ctime(&now);
    f << ((std::string)dt + " - Make TCP socket") << std::endl << std::endl;

    if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::cout << "Error: falied connect";
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Connection error") << std::endl << std::endl;
        return 1;
    }

    else {
        std::cout << "Connect with server!";
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Server was conected. Socket: ") << Connection << std::endl << std::endl;

        // если FoundUserData, то мы введем данные автоматически, но только единожды
        char confirm[256];

        if (FoundUserData(f1)) {
            std::cout << "Trying automatic authorization...\n";
            send(Connection, a_login, sizeof(a_login), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Send to server username: ") << a_login << std::endl << std::endl;

            send(Connection, a_passw, sizeof(a_passw), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Send to server password: ") << a_passw << std::endl << std::endl;

            recv(Connection, confirm, sizeof(confirm), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Authorisation ") << confirm << std::endl << std::endl;

            if (strcmp(confirm, "Accept") != 0) { std::cout << "wrong preloaded user data from config. Switching to manual autorization.\n"; }
        }

        char username[256];
        char password[256];

        while (strcmp(confirm, "Accept") != 0) {
            std::cout << "Username: ";
            std::cin >> username;
            std::cout << "Password: ";
            std::cin >> password;
            send(Connection, username, sizeof(username), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Send to server username: ") << username << std::endl << std::endl;

            send(Connection, password, sizeof(password), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Send to server password: ") << password << std::endl << std::endl;

            recv(Connection, confirm, sizeof(confirm), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Authorisation ") << confirm << std::endl << std::endl;
        }
        char figure = 'X';
        while (true) {
            char msg[20];
            recv(Connection, msg, 20, NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Server send message: ") << msg << std::endl << std::endl;
            if (strcmp(msg, "O") == 0) {
                figure = 'O';
                std::cout << "Your figure: " << figure;
                continue;
            }
            else if (strcmp(msg, "X") == 0) {
                figure = 'X';
                std::cout << "Your figure: " << figure;
                continue;
            }
            else if (strcmp(msg, "endX") == 0) {
                if (figure == 'X') {
                    printf("You are winner!\n");
                }
                else {
                    printf("You are loser!\n");
                }
                printf("Game finish.\n\nWould You Like To Play Again? Yes/ No: ");
                std::string otv;
                std::cin >> otv;
                if (otv == "Yes") {
                    send(Connection, "Yes", 4, NULL);
                    now = time(0); dt = ctime(&now);
                    f << ((std::string)dt + " - We send message: ") << "Yes" << std::endl << std::endl;
                }
                else {
                    send(Connection, "No", 3, NULL);
                    now = time(0); dt = ctime(&now);
                    f << ((std::string)dt + " - We send message: ") << "Yes" << std::endl << std::endl;
                    break;
                }
                continue;
            }
            else if (strcmp(msg, "endO") == 0) {
                if (figure == 'O') {
                    printf("You are winner!\n");
                }
                else {
                    printf("You are loser!\n");
                }
                printf("Game finish.\n\nWould You Like To Play Again? Yes/ No: ");
                char otv[4];
                scanf("%s", &otv);
                if (otv == "Yes") {
                    send(Connection, "Yes", 4, NULL);
                    now = time(0); dt = ctime(&now);
                    f << ((std::string)dt + " - We send message: ") << "Yes" << std::endl << std::endl;
                }
                else {
                    send(Connection, "No", 3, NULL);
                    now = time(0); dt = ctime(&now);
                    f << ((std::string)dt + " - We send message: ") << "Yes" << std::endl << std::endl;
                    break;
                }
                continue;
            }
            memcpy(board, msg, 20);
            char ans[4];
            int move = get_move();
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - We choice move: ") << move << std::endl << std::endl;
            board[move - 1] = figure;
            sprintf(ans, "%d", move);
            send(Connection, ans, sizeof(ans), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - We send message: ") << ans << std::endl << std::endl;
        }
        printf("Thanks for this game!");
    }
    f.close();
    return 0;

}
