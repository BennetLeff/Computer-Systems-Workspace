// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT 8080

int done = 0;

//thread work function
void* handle_connection(void* socket_pointer){
    int valread;

    char received_message_buffer[1024] = {0}; 

    int* sock = (int*)socket_pointer;

    bool killed = false;


    printf("Inside socket: sockid = %d\n", *sock);

    while (!killed) 
    {
        valread = read( *sock , received_message_buffer, 1024); 

        sleep(1);


        /// killserver prevents new connections
        if (strcmp(received_message_buffer, "killserver") == 0 || strcmp(received_message_buffer, "killserver\n") == 0)
        {
            done = 1;

            printf("%d$ killserver... done... \n", *sock);
        }

        // kill ends the clients connection... 
        if (strcmp(received_message_buffer, "kill") == 0 || strcmp(received_message_buffer, "kill\n") == 0)
        {
            printf("%d$ kill.... \n", *sock);
            killed = true;
        }

        else 
        {
            // we need append an id to the output so we need to create a second buffer
            char string_sent_to_socket[1024] = {0};

            // append the id to the output
            sprintf(string_sent_to_socket, "%d$ %s", *sock, received_message_buffer);

            // send the id and message back to the client
            send(*sock , string_sent_to_socket , strlen(string_sent_to_socket) , 0 ); 

            // clear the buffer
            memset(string_sent_to_socket, 0, 1024);
        }

        // clear the buffer
        memset(received_message_buffer, 0, 1024);
    }

    close(*sock);

    printf("Socket closed... \n");

}

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
       
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

    // spawn threads inside
    while (done == 0)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                           (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            // exit(EXIT_FAILURE); 

        }

         //start a thread if there was no error
        // pthread_t c_thread;
        // if(pthread_create(&c_thread, NULL, handle_connection, &new_socket) ) 
        // {
        //     fprintf(stderr, "Error creating thread\n");
        //     return 1;
        // }

        pid_t proc_id = fork();

        if (proc_id == 0)
        {
            // inside of child
            handle_connection(&new_socket);
        }

    }

    printf("Exiting... \n");

    return 0; 
} 