/**************************************************************
 * File:		ipk-client.cpp
 * Author:		Šimon Stupinský
 * University: 	Brno University of Technology
 * Faculty: 	Faculty of Information Technology
 * Course:	    Computer Communications and Networks	
 * Date:		02.03.2018
 * Last change:	12.03.2018	
 *		
 * Subscribe:	Client module for communication with server.
 *
 **************************************************************/

/**
 * @file ipk-client.cpp
 * @brief Client module for communication with server.
 */

#include "interface.h"

using namespace std;

/**
 * @brief   Establishing the connection with TCP Server, which
 *          has defined specifically identification (see params).
 *
 * @param   port    Destination port number in the range (0,65535)
 * @param   name    IP address or fully-qualified DNS name, identifying 
 *                  the server as the client communication endpoint
 * 
 * @return  int     Returns 0, if connection has been successfully established  
 */
int Socket::ConnectToTCPServer(int port, const char* name) {

    // create socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

    // setup socket
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    if ( (hp = gethostbyname(name)) == NULL ) {
        fprintf(stderr,"ERROR: no such host as %s\n", name);
        exit(EXIT_FAILURE);      
    }
    memcpy(&local.sin_addr, hp->h_addr, hp->h_length);

    // connect to server
    if (connect(socket_fd, (const struct sockaddr *)&local, len) != 0) {
		perror("ERROR: connect");
        close(socket_fd);
		exit(EXIT_FAILURE);        
    }

    return 0;
}

/**
 * @brief   Print help of this module and exit.
 * 
 */
void print_help() {

    fprintf(stderr, "Usage : $ ./ipk-client -h <host> -p <port> [-n|-f|-l] login\n\n");
    fprintf(stderr, "MANDATORY OPTIONS:\n");
    fprintf(stderr, "-h     host -> IP address or fully-qualified DNS name, identifying the server\n");
    fprintf(stderr, "-p     port -> Destination port number in the range (0,65535)\n");
    fprintf(stderr, "login  Login specifies the login name for the following operation\n\n");
    fprintf(stderr, "OPTIONAL OPTIONS: (1/3)\n");
    fprintf(stderr, "-n     name -> The full user name is returned, including any additional information for that login (User ID Info)\n");
    fprintf(stderr, "-f     file -> Return home directory information for the specified login (Home directory)\n");
    fprintf(stderr, "-l     list -> If the login was entered, will be searching users with the same prefix.\n");
    fprintf(stderr, "               If the login was not entered, will be returned all users.\n\n");
    fprintf(stderr, "EXAMPLES OF USAGE: $ ./ipk-client -h eva.fit.vutbr.cz -p 55555 -n rysavy\n");
    fprintf(stderr, "                   $ ./ipk-client -h host -p port -l\n");
    exit(EXIT_SUCCESS); 
}

/**
 * @brief   Checks the correctness of the arguments. 
 *          (checks allowed combinations and the right count)
 * 
 * @param   argc_       The count of input arguments entered from the command line
 * @param   argv_       The array with input arguments entered from the commane line
 * @param   port_number Variable for storing number of port, which was entered by user
 * @param   host_name   Variable for storing identification of server, which was entered by user
 * @param   login       Variable for storing login, which was entered by user for searching.
 * 
 * @return  int         Returns action which is required by client from the server.
 */
int check_arg(int argc_, char **argv_, int *port_number, char **host_name, char **login) {

    // disconnection of error messages of getopt function 
    extern int opterr;
    opterr = 0;

    bool set_mode = false;
    bool set_name = false;
    bool set_port = false;
    int flag = 0;
    int opt;
    while ((opt = getopt(argc_, argv_, "h:p:n:f:l:")) != -1) {
        switch(opt) {
            // host argument
            case 'h':
                if (set_name)   // check of duplicity
                    print_help();    
                *host_name = optarg;
                set_name = true;
                break;
            // port argument
            case 'p':
                if (set_port)   // check of duplicity
                    print_help();
                // check of allowed range of port number
                if (0 <= atoi(optarg) && atoi(optarg) <= 65535)
                    *port_number = atoi(optarg);
                else
                    print_help();
                set_port = true;
                break;
            // USER ID Info argmuent
            case 'n':
                if (set_mode) // check of duplicity 
                    print_help();
                set_mode = true;
                flag = 10;
                *login = optarg;
                break;
            // Home Directory argument  
            case 'f':
                if (set_mode) // check of duplicity  
                    print_help();
                set_mode = true;
                flag = 20;
                *login = optarg;
                break;  
            // List of USERS argument
            case 'l':
                if (set_mode) // check of duplicity
                    print_help();
                *login = optarg;
                flag = 30;
                set_mode = true;
                break;
            // optional options at -l argument, ignore mandatory
            case '?':
                if (optopt != 'l')
                    print_help();
                set_mode = true;
                *login = NULL;
                flag = 30;
                break;
            default:
                print_help();
        }
    }
    // check the presence of all type mandatory arguments and its right count
    if (argc_ < 6 || !set_mode || !set_port || !set_name || argc_> 7)
        print_help();

    return flag;
}

/**
 * @brief   Execution the relevant actions by type of the received
 *          message from the server. It also checks the correctness 
 *          type of the received message according to the required type.
 * 
 * @param   pc_connection   Socket representing 'ClientSocket', which is connected with server
 * @param   msg             Actual received messages from the server
 * @param   exp_type        Required type of message in this concept of communication
 * @param   notice          Useful information to be sent to server at unexpected type of message
 * 
 * @return  int             Returns 0, if action was successful executed.
 *                          Exit with code 1, if the message types do not match
 */
int exec_msg(Socket *pc_connection, message msg, msg_type exp_type, const char* notice) {

    // correct identity
    if (msg.type == DATA_END) 
        exp_type = DATA_END;

    // the message types do not match, but message is not of error type
    if (msg.type != exp_type && msg.type != ERROR) {
        // send error message to the server
        strncpy(msg.data, notice, sizeof(notice));
        msg.type = ERROR;
        pc_connection->SendTCPMessage((char*)&msg, sizeof(msg));
        // error notice at client side
        fprintf(stderr, "Unexpected type of the recieved message from server!\n");
        exit(1);
    }
    else { // execution the relevat actions by type of message
        switch(msg.type) {
            case ACCEPT:
                break;
            case DATA:
            case DATA_END:
                printf("%s", msg.data);
                break;
            case ERROR:
                fprintf(stderr, "%s", msg.data);
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "Unexpected type of the recieved message from server!\n");
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}

int main(int argc, char **argv) {

    int port_number;
    char *host_name;
    char *login;

    /* checks the correctness of the arguments. 
       (checks allowed combinations and the right count) */
    int flag = check_arg(argc, argv, &port_number, &host_name, &login);

    // connect to the server
    Socket* ClientSocket = new Socket();
    ClientSocket->ConnectToTCPServer(port_number, host_name);

    message client_msg;
    int res;

    // sending first introduction message to the server
    memset(client_msg.data, '\0', sizeof(client_msg.data));
    client_msg.type = BEGIN_CODE;
    ClientSocket->SendTCPMessage((char*)&client_msg, sizeof(client_msg));

    // waiting for the message with accepted answer from the server
    res = ClientSocket->ReceiveTCPMessage((char*)&client_msg, sizeof(client_msg));
    if (res <= 0) {
        ClientSocket->CloseSocket();
        delete(ClientSocket);
        exit(1);
    }
    exec_msg(ClientSocket, client_msg, ACCEPT, "I'm leaving, I'll see you later.\n");

    // set relevant type of the requirements for the server
    client_msg.type = (msg_type)flag;
    // set relevant capacity of the message
    int capacity = sizeof(client_msg.data)-1;
    int size = 0;
    if (login != NULL && login[0] != '\0') { // login was entered by user
        for (size; size < strlen(login); size += BUFF_SIZE-1) { // the last part of outgoing data
            if (size+BUFF_SIZE-1 >= strlen(login)) {
                client_msg.type = (msg_type)(++flag); 
                capacity = strlen(login) - size;
            }
            strncpy(client_msg.data, login+size, capacity);
            ClientSocket->SendTCPMessage((char*)&client_msg, sizeof(client_msg));
            memset(client_msg.data, '\0', sizeof(client_msg.data));
        }
    }
    else { // login was not entered by user, then send only relevant type of the requirements
        memset(client_msg.data, '\0', sizeof(client_msg.data));
        client_msg.type = (msg_type)(++flag);
        ClientSocket->SendTCPMessage((char*)&client_msg, sizeof(client_msg));
    }    

    for (;;) { // receiving and processing of server messages 
        res = ClientSocket->ReceiveTCPMessage((char*)&client_msg, sizeof(client_msg));
        if (res <= 0) break;
        exec_msg(ClientSocket, client_msg, DATA, "Unexpected type of message, I accept only DATA!\n");
        if (client_msg.type == DATA_END || client_msg.type == ERROR)
            break;
    };

    // add ending character
    if (flag == (int)NAMES_END || flag == (int)HOME_DIR_END)
        printf("\n");

    ClientSocket->CloseSocket();
    delete(ClientSocket);

    return 0;
}