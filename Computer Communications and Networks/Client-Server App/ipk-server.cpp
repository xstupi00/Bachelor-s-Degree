/**************************************************************
 * File:		ipk-server.cpp
 * Author:		Šimon Stupinský
 * University: 	Brno University of Technology
 * Faculty: 	Faculty of Information Technology
 * Course:	    Computer Communications and Networks	
 * Date:		02.03.2018
 * Last change:	12.03.2018	
 *		
 * Subscribe:	Server module for communication with clients.
 *
 **************************************************************/

/**
 * @file ipk-server.cpp
 * @brief Server module for communication with clients.
 */

#include "interface.h"

using namespace std;

/**
 * @brief   Creating the server with the relevant number of port and
 *          required address. Provides basically necessary operations
 *          to create a server.
 * 
 * @param   port        The port number on which the server listens for connections from clients
 * @param   address     Concrete address of server for connection (identification of server)
 * 
 * @return  int         Returns 0, if all operations was successfully executed
 */
int Socket::StartTCPServer(int port, int address) {

    // create socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {        
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

    // setup socket
    local.sin_family = AF_INET;
	local.sin_addr.s_addr = address;
	local.sin_port = htons(port);

    // bind socket to address
    if ((bind(socket_fd, (struct sockaddr*)&local, len)) < 0) {
		perror("ERROR: bind");
		exit(EXIT_FAILURE);		
	}

    // listen for connections on a socket
    if ((listen(socket_fd, LISTEN_QUEUE_NUMBER)) < 0) {
		perror("ERROR: listen");
		exit(EXIT_FAILURE);				
	}

    /* sets the current address to which the socket socket_fd 
       is bound, in the buffer pointed to by struct local */
    if (getsockname(socket_fd, (struct sockaddr *)&local, &len) < 0) {
        perror("ERROR: getsockname");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/**
 * @brief           Creating the connection with clients for communication with them
 * 
 * @return Socket*  Returns the newly created Socket, which representing the 
 *                  connection between server and client                
 */
Socket* Socket::AcceptTCPConnection() {

    // length of the string form for IPv4 (16)
    char str[INET_ADDRSTRLEN];

    Socket *new_socket = NULL;
    // accept a connection on a socket
    int accept_connection_fd = accept(socket_fd, (struct sockaddr *)0, (socklen_t*)0);
    // unsuccessful connection
    if (accept_connection_fd != -1)
        new_socket = new Socket(accept_connection_fd);

    return new_socket;
}

/**
 * @brief   Print help of this module and exit.
 * 
 */
void print_help() {

    fprintf(stderr, "Usage : $ ./ipk-server -p <port>\n\n");
    fprintf(stderr, "MANDATORY OPTION:\n");
    fprintf(stderr, "-p     port -> Destination port number in the range (0,65535)\n\n");
    fprintf(stderr, "EXAMPLE OF USAGE: $ ./ipk-server -p 55555\n");
    exit(EXIT_SUCCESS);
}

/**
 * @brief  Obtains the required informations by clients from /etc/passwd
 *         about specific users. 
 * 
 * @param   login   Login of user, about which it will be returning required informations
 * @param   flag    Mode selected by the client:
 *                  NAMES_END ->  the full user name is returned, including any additional 
 *                                information for that login (User ID Info)
 *                  HOMES_DIR ->  return home directory information for the specified login (Home directory)
 * 
 * @return  const char*     Returns obtained information for the specified login by selected mode
 */

/***********************************
 * Title: getpwnam(3)
 * Author: Linux (man page)
 * Date: UNKNOWN
 * Avaliability: https://linux.die.net/man/3/getpwnam
************************************/

const char* users_info(const char* login, int flag) {

    struct passwd pwd;
    struct passwd *result;
    char *buf;
    size_t bufsize;
    int check;

    // get size configuration at run time
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    // check if value was determinate 
    if (bufsize == -1)
    // indeterminate, should be more than enough
        bufsize = 16384;

    buf = (char*)malloc(bufsize);
    if (buf == NULL) {
        perror("ERROR: malloc");
        exit(EXIT_FAILURE);
    }

    // obtains information for the specied login by selected mode
    check = getpwnam_r(login, &pwd, buf, bufsize, &result);
    // the matching entry is not found or an error occurs
    if (result == NULL) {
        // the matching entry was not found
        if (check == 0) 
            return "Login was not found!\n";
        // some error has been occured
        else {
            errno = check;
            perror("ERROR: getpwnam_r");
        }
        exit(EXIT_FAILURE);
    }

    // return found User ID Info 
    if (flag == NAMES_END)
        return pwd.pw_gecos;
    // return found Home Directory
    else if(flag == HOME_DIR_END)
        return pwd.pw_dir;

}

/**
 * @brief   Obtains list of users from /etc/passwd. 
 * 
 * @param   login   If the login was entered, will be searching users with the same prefix.
 *                  If the login was not entered, will be returned all users.
 * 
 * @return  std::string     Returns string, which contains all relevant login by specification of client
 */
std::string list_of_users(const char* login) {

    // related operations with opening the file
    ifstream file_d;
    file_d.open("/etc/passwd");
    if (!file_d) {
        perror("ERROR: open");
        exit(EXIT_FAILURE);
    }

    // temporary variables to work
    string line;
    std::string tmp;
    std::string result;

    // specific login was entered by client
    if (login != NULL && login[0] != '\0') {
        while (getline(file_d, line)) {                 // it gets the every line of the file
            int i = 0;
            for (i; line[i] != ':', strlen(login) < i, line[i] == login[i]; i++)
                tmp += line[i];
            if (line[i] == ':' && i == strlen(login))   // whole matches with login, add ending character
                result += tmp + "\n";
            else if (strlen(login) == i) {              // prefix matches with login, copy the rest of the login
                for (i; line[i] != ':'; i++)
                    tmp += line[i];
                result += tmp + "\n";
            }
            tmp.clear();
        }
    }
    else {  // will be returned all users
        while (getline(file_d, line)) {
            for (int i = 0; line[i] != ':'; i++)
                tmp += line[i];
            result += tmp + "\n";
            tmp.clear();
        }
    }
    file_d.close();

    return result;
}

/**
 * @brief   The function sends the obtained information to the client.
 * 
 * @param   pc_connection   Socket which provides communication with client.
 * @param   msg             The message to send to client, includes type and useful data
 * @param   data            The whole package of the obtained data, to be served to the client
 * 
 * @return  int     Returns 0, if it was done successfully (all was be sent)
 */
int communication(Socket *pc_connection, message msg, const char* data) {

    // set the type of outgoing message
    if (data != "Login was not found!\n")
        msg.type = DATA;
    else
        msg.type = ERROR;
    // set relevant capacity of the message
    int capacity = sizeof(msg.data)-1;
    int size = 0;
    if (data != NULL && data[0] != '\0') { // data contains some useful obtains infomations
        for (size; size < strlen(data); size += BUFF_SIZE-1) {
            if (size+BUFF_SIZE-1 >= strlen(data) && msg.type != ERROR) { // the last part of outgoing data
                msg.type = DATA_END;
                capacity = strlen(data) - size;
            }
            strncpy(msg.data, data+size, capacity);
            pc_connection->SendTCPMessage((char*)&msg, sizeof(msg));
            memset(msg.data, '\0', sizeof(msg.data));
        }
    }
    else {  // no useful data has not been found
        strcpy(msg.data, "Any information not found!\n");
        msg.type = ERROR;
        pc_connection->SendTCPMessage((char*)&msg, sizeof(msg));
    }
    return 0;
}

/**
 * @brief   Execution the relevant actions by type of the received
 *          message from the client. It also checks the correctness 
 *          type of the received message according to the required type.
 * 
 * @param   pc_connection   Socket representing 'pc_connection', which is connected with client
 * @param   msg             Actual received messages from the client
 * @param   exp_type        Required type of message in this concept of communication
 * @param   notice          Useful information to be sent to client at unexpected type of message
 *                          or contains login entered by client for specific actions executed by server
 * 
 * @return  int             Returns 0, if action was successful executed.
 *                          Exit with code 1, if the message types do not match
 */
int exec_msg(Socket *pc_connection, message msg, msg_type exp_type, const char* notice) {

    // the message types do not match, but message is not of error type
    if (msg.type != exp_type && msg.type != ERROR) {
        // send error message to the client
        strncpy(msg.data, notice, sizeof(notice));
        msg.type = ERROR;
        pc_connection->SendTCPMessage((char*)&msg, sizeof(msg));
        // error notice at server side
        fprintf(stderr, "Unexpected type of the recieved message from client!\n");
    }
    else {  // execution the relevat actions by type of message
        switch(msg.type) {
            case BEGIN_CODE: {
                memset(msg.data, '\0', sizeof(msg.data));
                msg.type = ACCEPT;
                pc_connection->SendTCPMessage((char*)&msg, sizeof(msg));
            }
                break;
            case NAMES_END:
            case HOME_DIR_END: {
                const char* result = users_info(notice, msg.type);
                communication(pc_connection, msg, result);
            }
                break;     
            case USERS_END: {
                std::string data = list_of_users(notice);
                communication(pc_connection, msg, data.c_str());
            }
                break;
            case ERROR:
                fprintf(stderr, "%s\n", msg.data);
                break;
            default:
                fprintf(stderr, "Unexpected type of the recieved message from client!\n");   
                break;
        }
    }

    return 0;
}

int main(int argc, char** argv) {

    /* checks the correctness of the arguments. 
       (checks allowed combinations and the right count) */
    int port_number;
    if (argc == 3) {
        if (strcmp(argv[1], "-p") == 0 && 0 <= atoi(argv[2]) && atoi(argv[2]) <= 65535)
            port_number = atoi(argv[2]);
        else
            print_help();
    }
    else
        print_help();

    // begin listening    
    Socket* TCPServer = new Socket();
    TCPServer->StartTCPServer(port_number, INADDR_ANY);

    int res;
    while(1) {
        // accept connection with client side
        Socket* pc_connection = TCPServer->AcceptTCPConnection();

        // children process will communicate with client and serve his requirements
        int pid = fork();
        if (pid == 0) {
            message server_msg;
            std::string temp;

            // waiting for first message from client and sending connect-acception message
            res = pc_connection->ReceiveTCPMessage((char*)&server_msg, sizeof(server_msg));
            if (res <= 0) {
                pc_connection->CloseSocket();
                delete(pc_connection);
                // exit of children process
                exit(0);   
            }
            exec_msg(pc_connection, server_msg, BEGIN_CODE, "You haven't permission to connect!\n");

            for(;;) {   // receiving and processing of clients messages 
                res = pc_connection->ReceiveTCPMessage((char*)&server_msg, sizeof(server_msg));
                if (res <= 0) break;
                if (server_msg.type >= NAMES && server_msg.type <= USERS_END)
                    temp += server_msg.data;
                else
                   exec_msg(pc_connection, server_msg, USERS, "Unkown request!\n");
                if (server_msg.type % 10 == 1 || server_msg.type == ERROR)
                    break;
            }
            /* performance of activities according to the client's requirements
               and then re-send the obtained data */
            if (res > 0)
                exec_msg(pc_connection, server_msg, server_msg.type, temp.c_str());     

            // closing connection with client
            pc_connection->CloseSocket();
            delete(pc_connection);
            // exit of children process
            exit(EXIT_SUCCESS);
        }
        else // unsucsessfull creating a new process
            if (pid == -1) {
                perror("ERROR: fork");
                pc_connection->CloseSocket();
                delete(pc_connection);
                TCPServer->CloseSocket();
                delete(TCPServer);
                exit(1);
            }
            pc_connection->CloseSocket();
            delete(pc_connection);
    }
    return 0;
}