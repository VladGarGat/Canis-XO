#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#pragma warning(disable: 4996)

char board[20] = {};

void clearscreen(void) {
    for (int i = 0; i < 3; i++)
        printf("\n");
}


void clearboard(void) {
    for (int i = 0; i < 9; i++)
        board[i] = '-';
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


void print_board(char*) {
    printf("Game board\n");
    printf(" %c | %c | %c \n", board[0], board[1], board[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[3], board[4], board[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[6], board[7], board[8]);
}



//int get_move(void) {
//    printf("\nMenu of the moves:\n");
//    printf(" 1 | 2 | 3 \n");
//    printf("---+---+---\n");
//    printf(" 4 | 5 | 6 \n");
//    printf("---+---+---\n");
//    printf(" 7 | 8 | 9 \n");
//    printf("\n");
//    print_board((char*)":");
//    printf("\nEnter your move: ");
//    int move;
//    scanf("%d", &move);
//    while (move > 9 || move < 1 || board[move - 1] != '-') {
//        printf("Enter numder from 1 to 9:\n");
//        scanf("%d", &move);
//    }
//    return move;
//}


int get_opponent_move(SOCKET opponent, std::ofstream* f) {
    send(opponent, board, sizeof(board), NULL);
    time_t now = time(0); char* dt = ctime(&now);
    *f << ((std::string)dt + " - Send board and wait for move by ") << opponent << std::endl << std::endl;
    char otv[4];
    recv(opponent, otv, sizeof(otv), NULL);
    int move = atoi(otv);
    now = time(0); dt = ctime(&now);
    *f << ((std::string)dt + " - Recieve move ") << move << " by " << opponent << std::endl << std::endl;
    return move;
}
char play_and_get_winner(SOCKET opponents[2], std::ofstream* f, int move_time) {
    srand(time(NULL));
    int first = rand() % 2;
    SOCKET firstOp = opponents[first];
    SOCKET secondOp = opponents[(first + 1) % 2];
    time_t now = time(0); char* dt = ctime(&now);
    *f << ((std::string)dt + " - First move by ") << firstOp << std::endl << std::endl;

    now = time(0); dt = ctime(&now);
    *f << ((std::string)dt + " - Second move by ") << secondOp << std::endl << std::endl;


    send(firstOp, "X", sizeof("X"), NULL);
    now = time(0); dt = ctime(&now);
    *f << ((std::string)dt + " - Send figure X of playing to ") << firstOp << std::endl << std::endl;

    send(secondOp, "O", sizeof("O"), NULL);
    now = time(0); dt = ctime(&now);
    *f << ((std::string)dt + " - Send figure O of playing to ") << secondOp << std::endl << std::endl;

    int turn = 1;
    while (!has_wonQ('X') && !has_wonQ('O')) {
        clearscreen();
        now = time(0); dt = ctime(&now);
        *f << ((std::string)dt + " - Move ") << turn << std::endl << std::endl;

        if (turn % 2 == 1) {
            unsigned int start_time = clock();
            int move = get_opponent_move(firstOp, f);
            unsigned int end_time = clock();

            now = time(0); dt = ctime(&now);
            *f << ((std::string)dt + " - Time of move ") << end_time - start_time << "by" << firstOp << std::endl << std::endl;

            board[move - 1] = 'X';
            if (end_time - start_time > move_time) {
                now = time(0); dt = ctime(&now);
                *f << ((std::string)dt + " - Winner is ") << secondOp << "by time" << std::endl << std::endl;
                printf("O - winner \n");
                return 'O';
            }
            if (has_wonQ('X')) {
                now = time(0); dt = ctime(&now);
                *f << ((std::string)dt + " - Winner is ") << firstOp << std::endl << std::endl;
                printf("X - winner \n");
                return 'X';
            }
        }
        else {
            unsigned int start_time = clock();
            int move = get_opponent_move(secondOp, f);
            unsigned int end_time = clock();

            now = time(0); dt = ctime(&now);
            *f << ((std::string)dt + " - Time of move ") << end_time - start_time << "by" << secondOp << std::endl << std::endl;

            board[move - 1] = 'O';
            if (end_time - start_time > move_time) {
                now = time(0); dt = ctime(&now);
                *f << ((std::string)dt + " - Winner is ") << firstOp << "by time" << std::endl << std::endl;
                printf("X - winner \n");
                return 'X';
            }
            if (has_wonQ('O')) {
                now = time(0); dt = ctime(&now);
                *f << ((std::string)dt + " - Winner is ") << secondOp << std::endl << std::endl;
                printf("O - winner \n");
                return 'O';
            }
        }
        turn++;
        if (turn == 10) {
            now = time(0); dt = ctime(&now);
            *f << ((std::string)dt + " - Draw ") << std::endl << std::endl;
            printf("Draw!\n");
            return 'D';
        }
    }
}

int main(int argc, char* argv[]) {


    // В командую строку передается любое кол-во аргументов.
    // Название файла конфигурации должно быть подано в виде:
    // -cfg config_name

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

    std::string line;
    unsigned int move_time;
    std::unordered_map<std::string, std::string> users;
    std::getline(f1, line);
    move_time = stoi(line);
    while (std::getline(f1, line)) {
        size_t pos = line.find(" ");
        std::string username = line.substr(0, pos);
        line.erase(0, pos + 1);
        users[username] = line;
    }
    time_t now = time(0);
    char* dt = ctime(&now);
    f << ((std::string)dt + " - Loaded config file") << std::endl << std::endl;
    f1.close();
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error" << std::endl << std::endl;
        exit(1);
    }
    now = time(0); dt = ctime(&now);
    f << ((std::string)dt + " - Sockets start up!") << std::endl << std::endl;
    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    now = time(0); dt = ctime(&now);
    f << ((std::string)dt + " - Make TCP socket") << std::endl << std::endl;
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    now = time(0); dt = ctime(&now);
    f << ((std::string)dt + " - Bind sockets to IP/port") << std::endl << std::endl;
    listen(sListen, SOMAXCONN);
    now = time(0); dt = ctime(&now);
    f << ((std::string)dt + " - Listen port by socket") << std::endl << std::endl;
    SOCKET FConnection;
    SOCKET SConnection;
    int sizeofaddr = sizeof(addr);
    int x_wins = 0, o_wins = 0, ties = 0;
    std::cout << "Wating for a clients.";
    FConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
    now = time(0); dt = ctime(&now);
    f << ((std::string)dt + " - First player was conected. Socket: ") << FConnection << std::endl << std::endl;
    SConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
    now = time(0); dt = ctime(&now);
    f << ((std::string)dt + " - Second player was conected. Socket: ") << SConnection << std::endl << std::endl;
    SOCKET Players[2] = { FConnection, SConnection };
    if (FConnection == 0 || SConnection == 0) {
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Connections error") << std::endl << std::endl;
        std::cout << "Error #2\n";
    }
    else {
        std::cout << "Clients connected!";
        char username1[256];
        char username2[256];
        char password1[256];
        char password2[256];
        recv(FConnection, username1, sizeof(username1), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - First player username: ") << username1 << std::endl << std::endl;

        recv(SConnection, username2, sizeof(username2), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Second player username: ") << username2 << std::endl << std::endl;

        recv(FConnection, password1, sizeof(password1), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - First player password: ") << password1 << std::endl << std::endl;

        recv(SConnection, password2, sizeof(password2), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Second player password: ") << password2 << std::endl << std::endl;

        auto search1 = users.find(username1);
        auto search2 = users.find(username2);
        while (search1 == users.end() || search1->second != password1) {
            send(FConnection, "Wrong", sizeof("Wrong"), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - First player wrong authorisation") << std::endl << std::endl;
            recv(FConnection, username1, sizeof(username1), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - First player username: ") << username1 << std::endl << std::endl;
            recv(FConnection, password1, sizeof(password1), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - First player password: ") << password1 << std::endl << std::endl;
            search1 = users.find(username1);
        }
        send(FConnection, "Accept", sizeof("Accept"), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - First player is authorised") << std::endl << std::endl;
        while (search2 == users.end() || search2->second != password2) {
            send(SConnection, "Wrong", sizeof("Wrong"), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Second player wrong authorisation") << std::endl << std::endl;

            recv(SConnection, username2, sizeof(username2), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Second player username: ") << username2 << std::endl << std::endl;

            recv(SConnection, password2, sizeof(password2), NULL);
            f << ((std::string)dt + " - Second player password: ") << password2 << std::endl << std::endl;

            search2 = users.find(username2);
        }
        send(SConnection, "Accept", sizeof("Accept"), NULL);
        now = time(0); dt = ctime(&now);
        f << ((std::string)dt + " - Second player is authorised") << std::endl << std::endl;

        char reply[4] = "Yes";
        char reply1[4] = "Yes";
        while (strcmp(reply, "Yes") == 0 && strcmp(reply1, "Yes") == 0) {
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Game started") << std::endl << std::endl;
            clearboard();
            char winner = play_and_get_winner(Players, &f, move_time);
            print_board((char*)"\t");
            switch (winner) {
            case 'X':
                x_wins++;
                break;
            case 'O':
                o_wins++;
                break;
            case 'D':
                ties++;
                break;
            }
            printf("\n\t*Winner Statistic*\nPlayer X:%d\n", x_wins);
            printf("Player O:%d\n", o_wins);
            printf("Ties:%d \n\n", ties);
            char end[20] = "end";
            memcpy(end + sizeof("end") * sizeof('e') - 1, &winner, sizeof(winner));
            send(FConnection, end, sizeof(end), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Send endgame to player: ") << FConnection << std::endl << std::endl;

            send(SConnection, end, sizeof(end), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Send endgame to player: ") << SConnection << std::endl << std::endl;

            recv(FConnection, reply, sizeof(reply), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Recieve answer: ") << reply << std::endl << std::endl;

            recv(SConnection, reply1, sizeof(reply1), NULL);
            now = time(0); dt = ctime(&now);
            f << ((std::string)dt + " - Recieve answer: ") << reply1 << std::endl << std::endl;

            while (strcmp(reply, "Yes") != 0 && strcmp(reply, "No") != 0) {
                send(FConnection, "Please Enter A Valid Reply , Yes / No:", sizeof("Please Enter A Valid Reply , Yes / No:"), NULL);
                recv(FConnection, reply, sizeof(reply), NULL);
            }
            while (strcmp(reply1, "Yes") != 0 && strcmp(reply1, "No") != 0) {
                send(SConnection, "Please Enter A Valid Reply , Yes / No:", sizeof("Please Enter A Valid Reply , Yes / No:"), NULL);
                recv(SConnection, reply1, sizeof(reply), NULL);
            }
        }
    }
    std::cout << "Players are disconnected. Good game!";
    f.close();
    return 0;
}
