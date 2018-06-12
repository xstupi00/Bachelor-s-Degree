# IPK - 2.project -> DNS Lookup Tool

### DNS Lookup Tool
```sh
./ipk-lookup [-h]
./ipk-lookup -s server [-T timeout] [-t type] [-i] name
```
**MANDATORY OPTIONS:**    
**-s     server** ->  DNS Server (IPv4 or IPv6 address) to which the query will be sent.   
**name**   The translated domain name, for the -t PTR parameter, the incoming program expects the IPv4 or IPv6 address.  

**OPTIONAL OPTIONS:**  
**-T     TIMEOUT**   -> Timeout (in seconds) for query, default value = 5s  
**-t     type**      -> Type of query: A (default), AAAA, NS, PTR, CNAME   
**-i     iterative** -> Enforcing of iterative resolution  

EXAMPLES OF USAGE:  
```sh
$ ./ipk-lookup -s 8.8.8.8 -t AAAA -i www.fit.vutbr.cz.
$ ./ipk-lookup -s 8.8.8.8 -t PTR -i 147.229.13.238
```
**Extension:**  
-> IPv6 communication    
-> Extension of querying all possible servers until a reply (not only one random server)   
-> three chance to successful at timeout expiration   

**Restrictions:**   
-> perhaps the bad written of <name>, in certain situations (see documentation example - PTR record (iterative) - IPv6)     
  
**Extra solution:**   
-> combined solution of iteratives query, nearly described in the documentation

### Author  
**Stupinský Šimon**

