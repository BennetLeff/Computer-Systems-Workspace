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

// #include <errno.h>
// #include <fcntl.h>

#define PORT 8080

int done = 0;


// static int pipe_fds[2];


//thread work function
void* handle_connection(void* socket_pointer){
    int valread;

    char received_message_buffer[1024] = {0}; 

    int* sock = (int*)socket_pointer;

    bool killed = false;

    printf("Accepted socket: sockid = %d\n", *sock);

    while (!killed) 
    {
        valread = read( *sock , received_message_buffer, 1024); 

        sleep(1);


        /// killserver prevents new connections
        if (strcmp(received_message_buffer, "killserver") == 0 || strcmp(received_message_buffer, "killserver\n") == 0)
        {
            printf("killing parent to pipe");
            kill (getppid(), 9);
        }

        // kill ends the clients connection... 
        // in the fork code this exits/terminates  the child
        if (strcmp(received_message_buffer, "kill") == 0 || strcmp(received_message_buffer, "kill\n") == 0)
        {
            // send the id and message back to the client
            send(*sock , received_message_buffer , strlen(received_message_buffer) , 0 ); 

            // clear the buffer
            memset(received_message_buffer, 0, 1024);

            killed = true;
            exit(42);
        }

        else 
        {
            // send the id and message back to the client
            send(*sock , received_message_buffer , strlen(received_message_buffer) , 0 ); 

            // clear the buffer
            memset(received_message_buffer, 0, 1024);
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
    int *temp_socket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 

    // pipe(pipe_fds);

    /* Set O_NONBLOCK flag for the read end (pfd[0]) of the pipe. */
    // if (fcntl(pipe_fds[0], F_SETFL, O_NONBLOCK) == -1) {
    //     fprintf(stderr, "Call to fcntl failed.\n"); exit(1);
    // }

    char pipe_msg_buffer[4] = {0};

    // this should be set to false when the killserver command is issued
    bool accept_new_connections = true;

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
        }

        if (accept_new_connections)
        {

            temp_socket = malloc(sizeof(int));
            *temp_socket = new_socket;

            pid_t proc_id = fork();

            if (proc_id == 0)
            {

                // inside of child
                char id_string_buffer[8] = {0};

                // append the id to the output
                sprintf(id_string_buffer, "%d", getpid());


                send(*temp_socket , id_string_buffer , strlen(id_string_buffer) , 0 ); 


                handle_connection(temp_socket);

            }

            else
            {
                // printf("Branched to read. \n");

                // int nread;

                // /* Close the write-end of the pipe. */
                // if (close(pipe_fds[1]) == -1) 
                // { /* Failed to close write end of pipe. */
                //     fprintf(stderr, "Parent: Couldn’t close write end of pipe.\n"); exit(1);
                // }

                // for (;;) {
                //     switch (nread = read(pipe_fds[0], pipe_msg_buffer, 4)) {
                //         case -1: /* Make sure that pipe is empty. */
                //         if (errno == EAGAIN) {
                //             // printf("Parent: Pipe is empty\n"); fflush(stdout);
                //             sleep(1); break;
                //         }
                //         else { /* Reading from pipe failed. */
                //             fprintf(stderr, "Parent: Couldn’t read from pipe.\n"); exit(1);
                //         }
                //         case  0: /* Pipe has been closed. */
                //             printf("Parent: End of conversation.\n"); fflush(stdout); exit(0);
                //         default: /* Received a message from the pipe. */
                //             // printf("Parent: Message -- %s\n", pipe_msg_buffer); fflush(stdout); 
                //             accept_new_connections = false;
                //             printf("stopped accepting new connections.... \n");
                //             break;
                //     } /* End of switch. */
                // } /* End of for loop. */                
            }
        }
    }

    return 0; 
} 