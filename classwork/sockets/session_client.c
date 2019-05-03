#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 
   
int main(int argc, char const *argv[]) 
{ 
    
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char buffer[1024] = {0}; 
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    

    sleep(1);

    // allocate space to read the process id sent from the server,
    // which will be the id of the process or thread as a unique identifier
    char process_id[1024] = {0};
    valread = read( sock , process_id, 8); 
    printf("received:%s",process_id);

    // vars for reading input
    char *line;
    size_t len = 0;
    ssize_t read_amount;

    bool kill = false;

    printf("Begin inputing text to the server.... \n");

    // read input
    while (!kill && (read_amount = getline(&line, &len, stdin)) != -1) {
        // printf("read:[%s]\n",line);
        // send the input text from client to server
        send(sock , line , strlen(line) , 0 ); 

        // kill ends the clients connection... 
        if (strcmp(line, "kill") == 0 || strcmp(line, "kill\n") == 0)
        {
            printf("killed .... ");
            kill = true;
            return 0;
        }

        // read the response
        valread = read( sock , buffer, 1024); 

        // print the reponse with the correct id
        printf("%s$ %s", process_id, buffer );

        // clear the buffer every iteration
        memset(buffer, 0, 1024);
    }

    return 0; 
} 