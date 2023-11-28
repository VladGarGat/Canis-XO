#pragma comment(lib, "ws2_32.lib")
#define __STDC_WANT_LIB_EXT1__ 1
#include <winsock2.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <ctime>
#pragma warning(disable: 4996)

char board[20] = {};

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

int main(int argc, char* argv[]) {
    std::ofstream f;
    f.open("log.txt");
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
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
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
        std::cout << "Username: ";
        char username[256];
        std::cin >> username;
        std::cout << "Password: ";
        char password[256];
        std::cin >> password;
        char confirm[256];
        send(Connection, username, sizeof(username), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Send to server username: ") << username << std::endl << std::endl;

        send(Connection, password, sizeof(password), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Send to server password: ") << password << std::endl << std::endl;

        recv(Connection, confirm, sizeof(confirm), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Authorisation ") << confirm << std::endl << std::endl;
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