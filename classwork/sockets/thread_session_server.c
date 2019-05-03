// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <stdbool.h>

#define PORT 8080

int done = 0;

bool accept_new_connections = true;

//thread work function
void* handle_connection(void* socket_pointer){
    printf("Handling connection... \n");

    int* sock = (int*)socket_pointer;

                // inside of child
    char id_string_buffer[8] = {0};

    // append the id to the output
    sprintf(id_string_buffer, "%ld", pthread_self());

    // send the process id first so that the client can append it to the right messages
    send(*sock , id_string_buffer , 8 , 0 ); 

    int valread;

    char received_message_buffer[1024] = {0}; 

    bool killed = false;


    while (!killed) 
    {
        valread = read( *sock , received_message_buffer, 1024); 

        // printf("Read a value from client... %s\n",received_message_buffer);

        /// killserver prevents new connections
        if (strcmp(received_message_buffer, "killserver") == 0 || strcmp(received_message_buffer, "killserver\n") == 0)
        {
            accept_new_connections = false;
        }

        // kill ends the clients connection... 
        // in the fork code this exits/terminates  the child
        if (strcmp(received_message_buffer, "kill") == 0 || strcmp(received_message_buffer, "kill\n") == 0)
        {
            // send the id and message back to the client
            send(*sock , received_message_buffer , 1024 , 0 ); 

            killed = true;
        }

        else 
        {
            // send the id and message back to the client
            send(*sock , received_message_buffer , 1024 , 0 ); 
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
    int* temp_socket;
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

        if (accept_new_connections)
        {
            //start a thread if there was no error
            pthread_t c_thread;

            temp_socket = malloc(sizeof(int));
            *temp_socket = new_socket;

            int thr = pthread_create(&c_thread, NULL, handle_connection, temp_socket);
            if(thr) 
            {
                fprintf(stderr, "Error creating thread\n");
                return 1;
            }
        }
    }

    printf("Exiting... \n");

    return 0; 
} 