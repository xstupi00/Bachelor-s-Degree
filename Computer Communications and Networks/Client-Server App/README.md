# IPK - 1.project -> Client-Server application

### IPK-Client  
```sh
$ ./ipk-client -h merlin.fit.vutbr.cz -p 9998 [-n|-f|-l] login
```
**MANDATORY OPTIONS:**    
**-h     host** -> IP address or fully-qualified DNS name, identifying the server  
**-p     port** -> Destination port number in the range (0,65535)  
**login**  Login specifies the login name for the following operation  

**OPTIONAL OPTIONS:** (1/3)    
**-n     name** -> The full user name is returned, including any additional information for that login (User ID Info)  
**-f     file** -> Return home directory information for the specified login (Home directory)  
**-l     list** -> If the login was entered, will be searching users with the same prefix.  
               If the login was not entered, will be returned all users.  

EXAMPLES OF USAGE: 
```sh
./ipk-client -h eva.fit.vutbr.cz -p 55555 -n rysavy
./ipk-client -h host -p port -l
```

### IPK-Server
```sh
$ ./ipk-server -p <port>
```
**MANDATORY OPTIONS:**  
**-p     port** -> Destination port number in the range (0,65535)

EXAMPLE OF USAGE: 
```sh
$ ./ipk-server -p 55555 
```
### Compilation  
**make**

### Example of run  
```sh
$ ./ipk-server -p 55555 
$ ./ipk-client -h 127.0.0.1 -p 55555 -n rysavy
```
### Author  
**Stupinský Šimon**

