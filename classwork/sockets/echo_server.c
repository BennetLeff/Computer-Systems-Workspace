// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

#define PORT 8080

int done = 0;

char received_message_buffer[1024] = {0}; 

//thread work function
void* handle_connection(void* socket_pointer){
    int valread;
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
    int* sock = (int*)socket_pointer;

    printf("Inside thread: sockid = %d\n", *sock);

    valread = read( *sock , received_message_buffer, 1024); 

    sleep(3);


    if (strcmp(received_message_buffer, "kill") != 0 && strcmp(received_message_buffer, "kill\n"))
    {
        // printf("buffer is filled with %s ...\n", buffer);

        printf("received_message_buffer %s .... \n", received_message_buffer);

        send(*sock , received_message_buffer , strlen(received_message_buffer) , 0 ); 
        printf("Message %s returned. \n", received_message_buffer);
    }

    else 
    {
        // finish
        send(*sock , "kill" , strlen("kill") , 0 ); 

        printf("Shutting Down... \n");
        done = 1;

        close(*sock);

        exit(0);
    }

    memset(received_message_buffer, 0, 1024);

    close(*sock);

}

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 


    while (done == 0)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                           (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            // exit(EXIT_FAILURE); 

        }

        valread = read( new_socket , received_message_buffer, 1024); 
        // printf("buffer inside while loop %s\n.... ", received_message_buffer ); 
        send(new_socket , received_message_buffer , strlen(received_message_buffer) , 0 ); 
        printf("Message sent to client. \n"); 

         //start a thread
        pthread_t c_thread;
        if(pthread_create(&c_thread, NULL, handle_connection, &new_socket) ) 
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    printf("Exiting... \n");

    return 0; 
} 