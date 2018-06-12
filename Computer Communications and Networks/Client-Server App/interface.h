/**************************************************************
 * File:		interface.h
 * Author:		Šimon Stupinský
 * University: 	Brno University of Technology
 * Faculty: 	Faculty of Information Technology
 * Course:	    Computer Communications and Networks	
 * Date:		02.03.2018
 * Last change:	12.03.2018	
 *		
 * Subscribe:	Interface providing joint declarations class, methods
 *              and objects., which are used by modules 'ipk-client.cpp' 
 *              and 'ipk-server.cpp'.
 *
 **************************************************************/

/**
 * @file    interface.h
 * @brief   Interface providing joint declarations class, methods
 *          and objects., which are used by modules 'ipk-client.cpp' 
 *          and 'ipk-server.cpp'.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#define LISTEN_QUEUE_NUMBER 5
#define BUFF_SIZE 256

enum msg_type {
    BEGIN_CODE = 01,
    ACCEPT = 02,
    NAMES = 10,
    HOME_DIR = 20,
    USERS = 30,
    NAMES_END = 11,
    HOME_DIR_END = 21,
    USERS_END = 31,
    DATA = 40,
    DATA_END = 41,
    ERROR = 99,
};

/**
 * @brief Structure used to communication
 *        between clients and server.
 * 
 * @param type    Type of the outgoing message
 * @param data    Useful data for sending 
 */
struct message {
    msg_type type;  
    char data[BUFF_SIZE];
};

/**
 * @brief Class makes using Berkeley sockets easy.
 * 
 */

/***********************************
 * Title: Socket class
 * Author: Ryan Lattrel
 * Date: 11/16/2012
 * Avaliability: https://www.egr.msu.edu//classes/ece480/capstone/fall12/group02/documents/Ryan-Lattrel_App-Note.pdf
************************************/

class Socket {

    public:
        Socket();
        Socket(int fd);
        ~Socket();
        
        int CloseSocket();
        int SendTCPMessage(char *msg, int msg_size);
        int ReceiveTCPMessage(char *buffer, int buffer_size);
        int StartTCPServer(int port, int address);
        int ConnectToTCPServer(int port, const char* name);
        Socket* AcceptTCPConnection();

    private:
        int socket_fd;
        sockaddr_in local;
        socklen_t len;
        hostent *hp;
};

/**
 * @brief Construct a new Socket:: Socket object
 * 
 */
Socket::Socket() {

    len = sizeof(local);
}

/**
 * @brief Construct a new Socket:: Socket object
 *        Create a socket given a file descriptor
 * 
 * @param fd    file descriptor
 */
Socket::Socket(int fd) {

    socket_fd = fd;
    len = sizeof(local);
}

/**
 * @brief Destroy the Socket:: Socket object
 * 
 */
Socket::~Socket() {

    this->CloseSocket();
}

/**
 * @brief Destroy the Socket
 * 
 * @return  int     Return 0 in the successfull case
 */
int Socket::CloseSocket() {

    close(socket_fd);
    return 0;
}

/**
 * @brief Send a TCP message on socket
 * 
 * @param   msg         The message which will be sent
 * @param   msg_size    The size of the sending message
 * 
 * @return  int         Returns the number of characters 
 *                      sent or ends with an error alert.
 */
int Socket::SendTCPMessage(char *msg, int msg_size) {

    int bytestx = send(socket_fd, msg, msg_size, 0);
    if (bytestx < 0) 
      perror("ERROR in sendto"); 
    else
        return bytestx;
}

/**
 * @brief Receive a TCP message on socket.
 * 
 * @param   buffer      Buffer on storage the receiving message
 * @param   buffer_size Size of the this buffer
 * 
 * @return  int       Returns the number of characters 
 *                    receive or ends with an error alert.   
 */
int Socket::ReceiveTCPMessage(char *buffer, int buffer_size) {

    int bytesrx = recv(socket_fd, buffer, buffer_size, 0);
    if (bytesrx < 0) 
        perror("ERROR in recvfrom");
    else
        return bytesrx;
}