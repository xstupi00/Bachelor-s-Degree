/**************************************************************
 * File:		ipk-lookup.h
 * Author:		Šimon Stupinský
 * University: 	Brno University of Technology
 * Faculty: 	Faculty of Information Technology
 * Course:	    Computer Communications and Networks
 * Date:		29.03.2018
 * Last change:	09.04.2018
 *
 * Subscribe:	The header module of the Lookup Tool.
 *
 **************************************************************/

/**
 * @file    ipk-client.cpp
 * @brief   The header module of the Lookup Tool.
 */

#ifndef IPK_LOOKUP_H
#define IPK_LOOKUP_H

#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "stack.h"

/**
 * @brief           The union for two variant of communication with DNS server.
 *
 * @param   IPv4    Structure for establishment the connection in the IPv4
 * @param   IPv6    Structure for establishment the connection in the IPv6
 */
union address{
    struct sockaddr_in dest_IPv4;   ///< IPv4 variant
    struct sockaddr_in6 dest_IPv6;  ///< IPv6 variant
};

/**
 * @brief           The structure contains all needed flags and other items for
 *                  creating the head of DNS messages for communication
 *                  with the DNS server.
 */
struct DNS_HEADER {

    unsigned short ID;          ///< 16 bit identifier assigned by the program

    unsigned char RD :1;        ///< Recursion Desired
    unsigned char TC :1;        ///< TrunCation
    unsigned char AA :1;        ///< Authorirative Answer
    unsigned char OPCODE :4;    ///< 4 bit field that species kind of query
    unsigned char QR :1;        ///< Query or Response message
    unsigned char RCODE  :4;    ///< 4 bit field is set as part of responses
    unsigned char CD :1;        ///< Checking Disabled
    unsigned char AD :1;        ///< Authenticated Data
    unsigned char Z  :1;        ///< Reserver for the future use
    unsigned char RA :1;        ///< Recursion Available

    unsigned short QDCOUNT;     ///< number of entries in the question section.
    unsigned short ANCOUNT;     ///< number of resource records in the answer section.
    unsigned short NSCOUNT;     ///< number of name server resource records in the authority records section.
    unsigned short ARCOUNT;     ///< number of resource records in the additional records section
};

/**
 * @brief           The structure for definition the Question format used to
 *                  carry the "question" in most queries.
 */
struct QUESTION_FORMAT {

    unsigned short QTYPE;       ///< two octet code which specifies the type of the query
    unsigned short QCLASS;      ///< two octet code that specifies the class of the query
};

/* Constant sized fields of query structure */
#pragma pack(push, 1)
/**
 * @brief           The structure for Resource record format. All sections
 *                  (answer, authority, additional) share the same format.
 */
struct RESOURCE_FORMAT {

    unsigned short TYPE;        ///< two octets containing one of the RR type codes
    unsigned short CLASS;       ///< two octets which specify the class of the data in the RDATA field
    unsigned int TTL;           ///< a 32 bit unsigned integer that specifies the time interval (in seconds)
    unsigned short RDLENGTH;    ///< 16 bit integer that specifies the length in octets of the RDATA field
};
#pragma pack(pop)


/**
 * @brief           The structure for better abstraction resources records.
 *                  Contains individually pointers to contents of records.
 */
struct DATA_FORMAT {
    unsigned char *NAME;        ///< domain name to which this resource record pertains
    struct RESOURCE_FORMAT *RESOURCE; ///< pointer to structure contains needed to processing resource records
    unsigned char *RDATA;       ///< variable length string of octets that describes the resource
};

/**
 * @brief           Function to print error messages when the error state
 *                  has occurred and to print users help message.
 *
 * @param err_code  Code, for recognized the kind of error notice
 *
 * @return          Function exit the program with relevant exit code
 */
int print_err(int err_code);

/**
 * @brief           Function transform domain name to the appropriate one
 *                  form, which is suitable for DNS communication.
 *
 * @param dns       String for stored the transform form of domain name
 * @param host      Domain name to transformation
 *
 * @return          function has no return value
 */
void to_dns_format(unsigned char *dns, unsigned char *host);

/**
 * @brief           Function transform IPv4 address to the appropriate one
 *                  form, which is suitable for DNS communication
 *
 * @param dns       String for stored the transform form of IPv4 address
 * @param host      IPv4 address to transformation
 *
 * @return          function has no return value
 */
void to_IPv4_format(unsigned char *dns, unsigned char *host);

/**
 * @brief           Function transform IPv6 address to the appropriate one
 *                  form, which is suitable for DNS communication
 *
 * @param dns       String for stored the transform form of IPv6 address
 * @param host      IPv6 address to transformation
 *
 * @return          function has no return value
 */
void to_IPv6_format(unsigned char *dns, unsigned char *host);

/**
 * @brief           Function decoding type of records to its relevant text form.
 *
 * @param type      Type of record to decoding
 *
 * @return          function return relevant text form of the type
 */
char *decode_record_type(int type);

/**
 * @brief           Function executing the backward transformation of
 *                  the domain name from the form of DNS communication to
 *                  form of normal way.
 *
 * @param reader    Pointer to actual position in the receiving message
 * @param buffer    Buffer which contains the receiving message
 * @param count     Index for the following shift in the buffer
 *
 * @return          function returns transformed domain name and needed parameters
 */
u_char *read_name(unsigned char *reader, unsigned char *buffer, int *count);

/**
 * @brief           Function representing main part of all the program. Executing
 *                  set of needed flags in DNS Header of messages, establishing communication
 *                  with the DNS server, send self creating DNS message and follows of course
 *                  receiving response from the side of DNS server. As well as processing of the
 *                  all type of records and its stored to the represented structured, eventually
 *                  their statements to user. Function is using at both way of querying, iterative
 *                  and recursive.
 *
 * @param server_address        Address of the server, to which will be send the DNS query
 * @param add_info              Stack for stored information from the Additional Records
 * @param servers_queue         Stack for stored server from the Authoritative Servers
 * @param my_domain_name        Domain name, or address (IPv4 or IPv6) to which queries are giving
 * @param my_record_type        Record type of DNS query (A, AAAA, NS, CNAME, PTR)
 * @param ip_flag               Flag for searching the IP address and its returning
 * @param recursion_flag        Flag of Recursion Desired (iterative or recursive)
 * @param communication         Flag for the type of communication (IPv4 or IPv6)
 *
 * @return                      function returns find IP address in the case of set ip_flag or NULL value
 */
char* my_get_host_by_name(char *server_address, stack_t *add_info, stack_t *servers_queue, char* my_domain_name, int my_record_type, int ip_flag, int recursion_flag, int communication, int finish);

/**
 * @brief           Function to loading and storing arguments from the command line
 *
 * @param arg_count count of the arguments
 * @param arg_array array with the arguments
 *
 * @return          function has no return value
 */
void parse_arg(int arg_count, char **arg_array);

/**
 * @brief           Function executing iterative querying to DNS server.
 */
void iterative_query();

#endif //IPK_LOOKUP_H
