/**************************************************************
 * File:		ipk-lookup.c
 * Author:		Šimon Stupinský
 * University: 	Brno University of Technology
 * Faculty: 	Faculty of Information Technology
 * Course:	    Computer Communications and Networks
 * Date:		29.03.2018
 * Last change:	09.04.2018
 *
 * Subscribe:	The main module of the Lookup Tool.
 *
 **************************************************************/

/**
 * @file    ipk-client.cpp
 * @brief   The main module of the Lookup Tool.
 */

#include "ipk-lookup.h"

char dns_server_address[256];   ///< DNS Server
int TIMEOUT = 5;                ///< TIMEOUT for querying
int record_type = 1;            ///< Record Type of query
int communication = -1;         ///< Type of communication
bool iterative = false;         ///< querying mode
bool final_answer = false;      ///< flag of obtaining the request information in iterative mode
bool correct_ans = false;       ///< flag of finding only incorrect type of records
char domain_name[256];          ///< domain name on which queries are giving
int pushed_severs = 0;          ///< count of authoritative servers from the one response
int chances = 2;

///< print error messages and users help
int print_err(int err_code) {

    switch (ntohs(err_code)) {
        case 0:     ///< users help
            fprintf(stderr, "IPK-LOOKUP.\n");
            fprintf(stderr, "Usage: ./ipk-lookup [-h]\n");
            fprintf(stderr,  "       ./ipk-lookup -s server [-T timeout] [-t type] [-i] name\n\n");
            fprintf(stderr, "MANDATORY OPTIONS:\n");
            fprintf(stderr, "    -s server     -> DNS Server (IPv4 or IPv6 address) to which the query will be sent.\n");
            fprintf(stderr, "       name       -> The translated domain name, for the -t PTR parameter, the incoming program expects the IPv4 or IPv6 address.\n");
            fprintf(stderr, "OPTIONAL OPTIONS:\n");
            fprintf(stderr, "    -T TIMEOUT    -> Timeout (in seconds) for query, default value = 5s\n");
            fprintf(stderr, "    -t type       -> Type of query: A (default), AAAA, NS, PTR, CNAME.\n");
            fprintf(stderr, "    -i iterative  -> Enforcing of iterative resolution.\n\n");
            fprintf(stderr, "Format of the output: <name> [ ]+ IN [ ]+<type> [ ]+<answer>\n\n");
            fprintf(stderr, "EXAMPLES OF USAGE: $ ./ipk-lookup -s 8.8.8.8 -t AAAA -i www.fit.vutbr.cz.\n");
            fprintf(stderr, "                   $ ./ipk-lookup -s 8.8.8.8 -t PTR -i 147.229.13.238\n");
            break;
        case 1:     ///< bad format of DNS messages
            printf("ERROR: DNS Server: Format Error\n");
            exit(1);
        case 2:     ///< internal error of DNS server
            printf("ERROR: DNS Server: Server Failure\n");
            exit(1);
        case 3:     ///< bad domain name for querying
            printf("ERROR: DNS Server: Non-Existent Domain\n");
            exit(1);
        case 4:     ///< internal error of program
            printf("ERROR: Internal Error of Program\n");
            exit(1);
        default:    ///< other error on DNS server
            printf("ERROR: DNS Server: RCode of error is %d (see [RFC6895][RFC1035]).\n", ntohs(err_code));
            exit(1);
    }

    exit(0);
}


///< transform domain name to DNS format
void to_dns_format(unsigned char *dns, unsigned char *host) {

    int lock = 0;
    strcat((char *) host, ".");

    for (int i = 0; i < strlen((char *) host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++)
                *dns++ = host[lock];
            lock++;
        }
    }
    *dns++ = '\0';
}

///< transform IPv4 addresses to DNS format
void to_IPv4_format(unsigned char *dns, unsigned char *host) {

    int end = strlen((char *)host)-1;
    for (int i = strlen((char *) host)-1; i >= 0; i--) {
        // copy individually parts between two dots
        if (host[i] == '.') {
            int cnt = 0;
            for (int j = i + 1; j <= end; j++, cnt++)
                *dns++ = host[j];
            *dns++ = '.';
            end -= cnt+1;
        }
    }
    for(int j = 0; end >= 0; j++, end--)
        *dns++ = host[j];
    // adding a similar domain to the highest level
    char *src = ".in-addr.arpa";
    for(int i = 0; i < strlen((char *)src); i++)
        *dns++ = src[i];
    *dns++ = '\0';
}

///< transform IPv6 addresses to DNS format
void to_IPv6_format(unsigned char *dns, unsigned char *host) {

    int count_colons = 0, count = 0;
    for (int i = 0; i < strlen((char *)host); i++)
        if (host[i] == ':')
            count_colons++;
    // obtains count of empty blocks, which are representing by ::
    int zero_blocks = 7 - count_colons;

    for (int i = strlen((char*)host)-1; i >= 0; i--) {
        // copy individually parts between two colons
        if (host[i] != ':') {
            *dns++ = host[i];
            *dns++ = '.';
            count++;
        }
        // correction to right form
        else {
            while (count < 4) {
                *dns++ = '0';
                *dns++ = '.';
                count++;
            }
            // adding empty blocks
            if (host[i+1] == ':') {
                for (int cnt = zero_blocks * 4; cnt > 0; cnt--) {
                    *dns++ = '0';
                    *dns++ = '.';
                }
            }
            count = 0;
        }
    }

    // adding a similar domain to the highest level
    char *src = "ip6.arpa";
    for(int i = 0; i < strlen((char *)src); i++)
        *dns++ = src[i];
    *dns++ = '\0';
}

///< decoding type of record
char *decode_record_type(int type) {

    if (type == 1)
        return "A";
    else if (type == 2)
        return "NS";
    else if (type == 5)
        return "CNAME";
    else if (type == 12)
        return "PTR";
    else if (type == 28)
        return "AAAA";
}

///< backward transformation domain name from the DNS form
u_char *read_name(unsigned char *reader, unsigned char *buffer, int *count) {

    unsigned char *name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char *) malloc(256);

    name[0] = '\0';

    // read the names in 3www6google3com format
    while (*reader) {
        if (*reader >= 192) {
            offset = (*reader) * 256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; // we have jumped to another location so counting wont go up!
        } else
            name[p++] = *reader;

        reader = reader + 1;

        if (!jumped)
            *count = *count + 1; // if we havent jumped to another location then we can count up
    }

    name[p] = '\0'; // string complete
    if (jumped == 1)
        *count = *count + 1; // number of steps we actually moved forward in the packet

    // now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int) strlen((const char *) name); i++) {
        p = name[i];
        for (j = 0; j < (int) p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0'; // remove the last dot

    return name;
}

///< main function executing the logic of communication (send, receive, parse)
char* my_get_host_by_name(char *server_address, stack_t *add_info, stack_t *servers_queue, char* my_domain_name, int my_record_type, int ip_flag, int recursion_flag, int communication, int finish) {

    // definition general variables needed to processing
    unsigned char buffer[65536], class[3], str_record_type[6], *tmpname, *qname, *reader;
    int end = 0, tmp, UDPSocket;
    socklen_t length;

    // definition of the structure needed to representing of DNS message
    struct DNS_HEADER *dns = NULL;
    struct DATA_FORMAT answers[64], auth[64], addit_IPv4[64], addit_IPv6[64];
    struct QUESTION_FORMAT *qinfo = NULL;

    // select the type of communication
    union address dest;
    if (communication == 0) {   ///< IPv4 communication
        struct sockaddr_in addr;
        length = sizeof(dest.dest_IPv4);

        // sets the parameters of connection
        UDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        dest.dest_IPv4.sin_family = AF_INET;
        dest.dest_IPv4.sin_port = htons(53);
        // check correctness of given IPv4 address
        if (inet_aton(server_address, &dest.dest_IPv4.sin_addr) == 0) {
            fprintf(stderr, "ERROR: Invalid IPv4 address.\n");
            exit(1);
        }
    }
    else {      ///< IPv6 communication
        struct sockaddr_in6 addr;
        length = sizeof(dest.dest_IPv6);

        // sets the parameter of connection
        UDPSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        dest.dest_IPv6.sin6_family = AF_INET6;
        dest.dest_IPv6.sin6_port = htons(53);
        // check correctness of given IPv4 address
        int c = inet_pton(AF_INET6, server_address, (void *)&dest.dest_IPv6.sin6_addr.s6_addr);
        if (c <= 0) {
            if (c == 0)
                fprintf(stderr, "IPv6 not in presentation format.\n");
            else // internal error of inet_pton
                perror("ERROR: inet_pton.\n");
            exit(1);
        }
    }

    // filling the flags of the DNS header
    dns = (struct DNS_HEADER *) &buffer;
    dns->ID = (unsigned short) htons(getpid()); // identification number
    dns->QR = 0;                    // it is a query
    dns->AA = 0;
    dns->TC = 0;
    dns->RD = recursion_flag;       // recursive or iterative
    dns->CD = 0;
    dns->AD = 0;
    dns->RA = 0;
    dns->Z = 0;
    dns->OPCODE = 0;
    dns->RCODE = 0;
    dns->QDCOUNT = htons(1);        // 1 question
    dns->ANCOUNT = 0;
    dns->ARCOUNT = 0;
    dns->NSCOUNT = 0;

    // processing the domain name or address (IPv4 or IPv6) to DNS format
    qname = (unsigned char *) &buffer[sizeof(struct DNS_HEADER)];
    tmpname = (unsigned char *) malloc(sizeof(struct DNS_HEADER));
    if (my_record_type != 12) {     // domain name
        if (strcmp(my_domain_name, ".")) {
            to_dns_format(qname, my_domain_name);
        }
    }
    else {      // addresses in PTR query type
        for (int i = 0; i < strlen((char *)my_domain_name); i++) {
            if (my_domain_name[i] == '.') {       // IPv4
                to_IPv4_format(tmpname, my_domain_name);
                to_dns_format(qname, tmpname);
                break;
            } else if (my_domain_name[i] == ':') { // IPv6
                to_IPv6_format(tmpname, my_domain_name);
                to_dns_format(qname, tmpname);
                break;
            }
        }
    }

    // sets parameters to structure representing a query
    qinfo = (struct QUESTION_FORMAT *) &buffer[sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1)];
    qinfo->QCLASS = htons(1);               // INTERNET
    qinfo->QTYPE = htons(my_record_type);   // relevant type of record

    // timeout for respnse to query from the server side
    struct timeval timeout = {TIMEOUT, 0};

    if (setsockopt (UDPSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0) {
        perror("ERROR: setsockopt");
        exit(EXIT_FAILURE);
    }

    // sending DNS query
    if (sendto(UDPSocket, (char *) buffer,
               sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1) + sizeof(struct QUESTION_FORMAT), 0,
               (struct sockaddr *) &dest, length) < 0) {
        perror("sendto failed");
        exit(EXIT_FAILURE);
    }

    tmp = sizeof dest;
    // receive DNS response
    if (recvfrom(UDPSocket, (char *) buffer, 65536, 0, (struct sockaddr *) &dest, (socklen_t *) &tmp) < 0) {
        fprintf(stderr, "\nERROR: Some error was occurred at receiving to Socket!\n");
        if (chances != 0) { // something wrong, try all process repeat again
            fprintf(stderr, "I'll try it once (%d chances)!\n\n", chances);
            chances--;
            iterative_query();
        }
        else { // the end, unsuccsesful
            fprintf(stderr, "\nERROR: Obtaining information about %s was not succesfull!\n", domain_name);
            exit(EXIT_FAILURE);
        }
    }

    // obtains DNS header from response
    dns = (struct DNS_HEADER *) buffer;
    // obtains DNS body from response
    reader = &buffer[sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1) + sizeof(struct QUESTION_FORMAT)];

    // something wrong on the server side
    if (ntohs(dns->RCODE) != 0)
        print_err(ntohs(dns->RCODE));

    // OBTAINING ANSWERS
    for (int i = 0; i < ntohs(dns->ANCOUNT); i++) {
        if (ip_flag == 0 && finish == 0) {
            final_answer = true;}
        answers[i].NAME = read_name(reader, buffer, &end);
        reader += end;

        answers[i].RESOURCE = (struct RESOURCE_FORMAT *) (reader);
        if (ntohs(answers[i].RESOURCE->TYPE) != 28)
            reader += sizeof(struct RESOURCE_FORMAT);
        else
            reader += sizeof(struct RESOURCE_FORMAT)-8;

        if (ntohs(answers[i].RESOURCE->TYPE) == 1 || ntohs(answers[i].RESOURCE->TYPE) == 28) {
            answers[i].RDATA = (unsigned char *) malloc(ntohs(answers[i].RESOURCE->RDLENGTH)+32);
            if(ntohs(answers[i].RESOURCE->TYPE) == 1) {
                for (int j = 0; j < ntohs(answers[i].RESOURCE->RDLENGTH); j++)
                    answers[i].RDATA[j] = reader[j];
                answers[i].RDATA[ntohs(answers[i].RESOURCE->RDLENGTH)] = '\0';
                reader += ntohs(answers[i].RESOURCE->RDLENGTH);
                if (ip_flag == 1)
                    return answers[i].RDATA;
            }
            else if(ntohs(answers[i].RESOURCE->TYPE) == 28) {
                for (int j = 0; j < ntohs(answers[i].RESOURCE->RDLENGTH) + 32; j++)
                    answers[i].RDATA[j] = reader[j];
                answers[i].RDATA[ntohs(answers[i].RESOURCE->RDLENGTH)+24] = '\0';
                reader += ntohs(answers[i].RESOURCE->RDLENGTH)+24;
            }
        } else {
            answers[i].RDATA = read_name(reader, buffer, &end);
            reader += end;
        }
        if(finish)
            S_Push(servers_queue, &answers[i]);
    }

    // PRINT ANSWERS
    if (finish == 0) {
        for (int i = 0; i < ntohs(dns->ANCOUNT); i++) {
            if (ntohs(answers[i].RESOURCE->CLASS) == 1)
                strcpy(class, "IN");
            strcpy(str_record_type, decode_record_type(ntohs(answers[i].RESOURCE->TYPE)));
            printf("%s %s %s ", answers[i].NAME, class, str_record_type);
            if (ntohs(answers[i].RESOURCE->TYPE) == 1) {
                long *p;
                p = (long *) answers[i].RDATA;
                struct sockaddr_in addr_IPv4;
                addr_IPv4.sin_addr.s_addr = (*p);
                printf("%s", inet_ntoa(addr_IPv4.sin_addr));
            }
            else if (ntohs(answers[i].RESOURCE->TYPE) == 28) {
                struct sockaddr_in6 *ipv6;
                char add[INET6_ADDRSTRLEN];
                ipv6 = (struct sockaddr_in6 *)answers[i].RDATA;
                void *addr1;
                addr1 = &(ipv6->sin6_addr);
                inet_ntop(AF_INET6, addr1, add, sizeof(add));
                printf("%s", add);
            }
            else
                printf("%s", answers[i].RDATA);

            if (ntohs(answers[i].RESOURCE->TYPE) == record_type)
                correct_ans = true;
            printf("\n");
        }
    }

    // OBTAINING NS SERVERS
    pushed_severs =  ntohs(dns->NSCOUNT);
    for (int i = 0; i < ntohs(dns->NSCOUNT); i++) {
        auth[i].NAME = read_name(reader, buffer, &end);
        reader += end;
        auth[i].RESOURCE = (struct RESOURCE_FORMAT *) reader;
        reader += sizeof(struct RESOURCE_FORMAT);
        auth[i].RDATA = read_name(reader, buffer, &end);
        reader += end;
        if (ip_flag == 0) {
            S_Push(servers_queue, &auth[i]);
        }
    }

    // OBTAINING ADDITIONAL
    unsigned char *save_reader = reader;
    int save_end = end;
    int cnt_IPv6 = 0;
    for (int i = 0; i < ntohs(dns->ARCOUNT); i++) {
        addit_IPv6[cnt_IPv6].NAME = read_name(reader, buffer, &end);
        reader += end;

        addit_IPv6[cnt_IPv6].RESOURCE = (struct RESOURCE_FORMAT *) (reader);
        if (ntohs(addit_IPv6[cnt_IPv6].RESOURCE->TYPE) != 28)
            reader += sizeof(struct RESOURCE_FORMAT);
        else
            reader += sizeof(struct RESOURCE_FORMAT)-8;

        if(ntohs(addit_IPv6[cnt_IPv6].RESOURCE->TYPE) == 1 || ntohs(addit_IPv6[cnt_IPv6].RESOURCE->TYPE) == 28) {
            addit_IPv6[cnt_IPv6].RDATA = (unsigned char*) malloc(ntohs(addit_IPv6[cnt_IPv6].RESOURCE->RDLENGTH)+32);
            if (ntohs(addit_IPv6[cnt_IPv6].RESOURCE->TYPE) == 1) {
                for (int j = 0; j < ntohs(addit_IPv6[cnt_IPv6].RESOURCE->RDLENGTH); j++)
                    addit_IPv6[cnt_IPv6].RDATA[j] = reader[j];
                addit_IPv6[cnt_IPv6].RDATA[ntohs(addit_IPv6[cnt_IPv6].RESOURCE->RDLENGTH)] = '\0';
                reader += ntohs(addit_IPv6[cnt_IPv6].RESOURCE->RDLENGTH);
            }
            else if (ntohs(addit_IPv6[cnt_IPv6].RESOURCE->TYPE) == 28) {
                for (int j = 0; j < ntohs(addit_IPv6[cnt_IPv6].RESOURCE->RDLENGTH)+32; j++)
                    addit_IPv6[cnt_IPv6].RDATA[j] = reader[j];
                addit_IPv6[cnt_IPv6].RDATA[ntohs(addit_IPv6[cnt_IPv6].RESOURCE->RDLENGTH)+24] = '\0';
                reader += ntohs(addit_IPv6[cnt_IPv6++].RESOURCE->RDLENGTH)+24;
            }
        }
        else {
            addit_IPv6[cnt_IPv6].RDATA = read_name(reader, buffer, &end);
            reader += end;
        }
    }

    reader = save_reader;
    end = save_end;
    int cnt_IPv4 = 0;
    for (int i = 0; i < ntohs(dns->ARCOUNT); i++) {
        addit_IPv4[cnt_IPv4].NAME = read_name(reader, buffer, &end);
        reader += end;

        addit_IPv4[cnt_IPv4].RESOURCE = (struct RESOURCE_FORMAT*)reader;
        reader += sizeof(struct RESOURCE_FORMAT);

        if(ntohs(addit_IPv4[cnt_IPv4].RESOURCE->TYPE) == 1) {
            addit_IPv4[cnt_IPv4].RDATA = (unsigned char*)malloc(ntohs(addit_IPv4[cnt_IPv4].RESOURCE->RDLENGTH)+32);
            for (int j = 0; j < ntohs(addit_IPv4[cnt_IPv4].RESOURCE->RDLENGTH); j++)
                addit_IPv4[cnt_IPv4].RDATA[j]= reader[j];
            addit_IPv4[cnt_IPv4].RDATA[ntohs(addit_IPv4[cnt_IPv4].RESOURCE->RDLENGTH)] = '\0';
            reader += ntohs(addit_IPv4[cnt_IPv4++].RESOURCE->RDLENGTH);
        }
        else if (ntohs(addit_IPv4[cnt_IPv4].RESOURCE->TYPE) == 28) {
            reader += ntohs(addit_IPv4[cnt_IPv4].RESOURCE->RDLENGTH);
        }
        else {
            addit_IPv4[cnt_IPv4].RDATA = read_name(reader, buffer, &end);
            reader += end;
        }
    }

    for (int i = 0; i < cnt_IPv4; i++) {
        if (ntohs(addit_IPv4[i].RESOURCE->TYPE) == 1) {
            long *p;
            p = (long *) addit_IPv4[i].RDATA;
            struct sockaddr_in addr_IPv4;
            addr_IPv4.sin_addr.s_addr = (*p);
            S_Push(add_info, &addit_IPv4[i]);
        }
    }

    for (int i = 0; i < cnt_IPv6; i++) {
            struct sockaddr_in6 *ipv6;
            char add[INET6_ADDRSTRLEN];
            ipv6 = (struct sockaddr_in6 *) addit_IPv6[i].RDATA;
            void *addr1;
            addr1 = &(ipv6->sin6_addr);
            S_Push(add_info, &addit_IPv6[i]);
            inet_ntop(AF_INET6, addr1, add, sizeof(add));
    }

    return NULL;
}



void iterative_query() {

    char class[3], str_record_type[6];
    struct DATA_FORMAT *addr_type;
    stack_t *servers_queue = S_Init();
    stack_t *add_info = S_Init();
    char *tmp = my_get_host_by_name(dns_server_address, add_info, servers_queue, ".", 2, 0, 0, communication, 1);
    add_info = S_Save(add_info);

    while (!final_answer) {
        struct DATA_FORMAT *auth_server;
        if (S_Empty(servers_queue))
            break;
        auth_server = (struct DATA_FORMAT *) S_Top(servers_queue);
        if (ntohs(auth_server->RESOURCE->CLASS) == 1)
            strcpy(class, "IN");
        strcpy(str_record_type, decode_record_type(ntohs(auth_server->RESOURCE->TYPE)));
        S_Pop(servers_queue);
        pushed_severs--;

        while (!S_Empty(add_info)) {
            addr_type = (struct DATA_FORMAT *) S_Top(add_info);
            S_Pop(add_info);
            if (!strcmp(addr_type->NAME, auth_server->RDATA))
                break;
        }

        if (!strcmp(auth_server->NAME, ""))
            strcpy(auth_server->NAME, ".");


        long *p;
        struct sockaddr_in addr;

        if (S_Empty(add_info)) {
            if (pushed_severs && !S_Empty(servers_queue)) {
                add_info = S_Comeback(add_info);
                continue;
            }
            else {
                stack_t *tmp_stack = S_Init();
                p = (long *) my_get_host_by_name(dns_server_address, tmp_stack, tmp_stack, auth_server->RDATA, 1, 1, 1, communication, 0);

                addr.sin_addr.s_addr = (*p);

                printf("%s %s A %s\n", auth_server->RDATA, class, inet_ntoa(addr.sin_addr));
                my_get_host_by_name(inet_ntoa(addr.sin_addr), add_info, servers_queue, domain_name, record_type, 0, 0, 0, 0);
            }
        }
        else if (ntohs(addr_type->RESOURCE->TYPE) == 1) {
            printf("%s %s %s %s\n", auth_server->NAME, class, str_record_type, auth_server->RDATA);
            p = (long *) addr_type->RDATA;

            addr.sin_addr.s_addr = (*p);

            printf("%s %s A %s\n", auth_server->RDATA, class, inet_ntoa(addr.sin_addr));
            my_get_host_by_name(inet_ntoa(addr.sin_addr), add_info, servers_queue, domain_name, record_type, 0, 0, 0, 0);
        }
        else if (ntohs(addr_type->RESOURCE->TYPE) == 28) {
            printf("%s %s %s %s\n", auth_server->NAME, class, str_record_type, auth_server->RDATA);
            struct sockaddr_in6 *ipv6;
            char add[INET6_ADDRSTRLEN];
            ipv6 = (struct sockaddr_in6 *) addr_type->RDATA;
            void *addr1;
            addr1 = &(ipv6->sin6_addr);
            inet_ntop(AF_INET6, addr1, add, sizeof(add));

            printf("%s %s AAAA %s\n", auth_server->RDATA, class, add);
            my_get_host_by_name(add, add_info, servers_queue, domain_name, record_type, 0, 0, 1, 0);
        }
        add_info = S_Comeback(add_info);
        S_Pop(servers_queue);
    }
}

void parse_arg(int arg_count, char **arg_array) {

    int opt;
    while ((opt = getopt(arg_count, arg_array, "hs:T:t:i")) != -1) {
        switch (opt) {
            case 'h':
                print_err(0);
                break;
            case 's':
                strcpy(dns_server_address, optarg);
                for (int i = 0; i < strlen(dns_server_address); i++) {
                    if (dns_server_address[i] == '.') {
                        communication = 0;
                        break;
                    }
                    else if (dns_server_address[i] == ':') {
                        communication = 1;
                        break;
                    }
                }
                if (communication == -1) {
                    fprintf(stderr, "ERROR: Invalid IPv4 address.\n");
                    exit(1);
                }
                break;
            case 'T':
                TIMEOUT = atoi(optarg);
                break;
            case 't':
                if (!strcmp(optarg, "A")) record_type = 1;
                else if (!strcmp(optarg, "AAAA")) record_type = 28;
                else if (!strcmp(optarg, "NS")) record_type = 2;
                else if (!strcmp(optarg, "PTR")) record_type = 12;
                else if (!strcmp(optarg, "CNAME")) record_type = 5;
                else {
                    fprintf(stderr, "ERROR: Wrong type of query. Allowed types are: A (default), AAAA, NS, PTR, CNAME.\n");
                    exit(2);
                }
                break;
            case 'i':
                iterative = true;
                break;
            default:
                fprintf(stderr, "Usage: ./ipk-lookup [-h]\n./ipk-lookup -s server [-T timeout] [-t type] [-i] name");
                exit(2);
        }
    }

    strcpy(domain_name, arg_array[arg_count - 1]);
}

int main(int argc, char **argv) {

    parse_arg(argc, argv);
    if (iterative)
        iterative_query();
    else {
        stack_t *tmp_stack = S_Init();
        my_get_host_by_name(dns_server_address, tmp_stack, tmp_stack, domain_name, record_type, 0, 1, communication, 0);
    }
    if (!correct_ans)
        return 1;

    return 0;
}