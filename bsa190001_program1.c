/****************************************************************************
    File: bsa190001_program1.c
    Author: Brandon Atwal
    Purpose: To create a client/server program in C using message queues which finds the
    domain name, host name or uname of the system per user request.
    Procedures:
        main    - forks the program into parent (server) and child (client) processes. 
        client  - takes user input, sends request to server, receives server results
                    outputs results to user.
        server  - receives request from server, finds requested data, sends results
                    back to client
*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


#define Q_C_NAME  "/pgm1client"
#define Q_S_NAME  "/pgm1server"
#define Q_MSG_SIZE    1024

typedef struct {
    char msg[Q_MSG_SIZE];
} MSG;


    

/**********************************************************************************
* int client()
* Author: Brandon Atwal
* Date: 02/07/2022
* Description: The client will take the input which is typed in the terminal
and send it to the server. When the server replies, it outputs the results to the user
**********************************************************************************/
int client()
{
    //queue attribute struct
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(MSG),
        .mq_curmsgs = 0
    };
    char message[Q_MSG_SIZE];
    int sent_value = 0;
    int read_value = 0;

    //Open client and server queues
    mqd_t que_client = mq_open(Q_C_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attr); //queue for client -> server. write only
    mqd_t que_server = mq_open(Q_S_NAME, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr); //queue for server -> client. read only
    if (que_client < 0 || que_server < 0) {
       perror("Error: Program Failed. Try Again.\n");
       exit(1);
    }

    MSG msg_to_send;
    //This while loop requests user input, sends message to server, get server response, output to user
    while (1)  {
        printf("\n Enter the number of the option you wish to view: \n"); 
        printf("1. Domain Name\n");
        printf("2. Hostname\n");
        printf("3. UNAME\n");
        printf("4. Exit\n");

        //empty the message then take new input
        strcpy(msg_to_send.msg, ""); 
        fgets(msg_to_send.msg, Q_MSG_SIZE, stdin);
        //printf("input: %s",(char *)&msg_to_send); //repeat input for error checking

        
        //Users input is sent to the server
        sent_value = mq_send(que_client, (char *)&msg_to_send.msg, Q_MSG_SIZE, 0);

        //If 4 is eneter the program will break the loop and exit 
        if(msg_to_send.msg[0] == '4'){
            break;
        }
        //If not 4, receive output then print to user
        MSG msg_to_output;
        read_value = mq_receive(que_server, msg_to_output.msg, Q_MSG_SIZE, NULL);

        //Print the output
        msg_to_output.msg[read_value] = '\0';
        printf("\n\n\n\n Output: %s", msg_to_output.msg);
    }

    //close and unlink queues
    mq_close(que_client);
    mq_close(que_server);
    mq_unlink(Q_C_NAME);
    mq_unlink(Q_S_NAME);

    return 0;
}

/**********************************************************************************
* int server()
* Author: Brandon Atwal
* Date: 02/07/2022
* Description: The input of the user is sent to the server from the client. The 
server will read and process the input and will send the appropriate output.
**********************************************************************************/
int server()
{
    //queue attribute struct
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(MSG),
        .mq_curmsgs = 0
    };
    //temp array for processing
    char temp[Q_MSG_SIZE/2];

    int sent_value;
    int read_value;
    struct utsname unameData;
    
    //open client and server queues
    mqd_t que_client = mq_open(Q_C_NAME, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr); //queue for client -> server. read only
    mqd_t que_server = mq_open(Q_S_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attr); //queue for server -> client. write only
    
    //Gets the input, gets proper output, sends it to client
    while(1) {
        MSG message;
        MSG output;
        read_value = mq_receive(que_client, (char *)&message.msg, Q_MSG_SIZE, NULL);

        //printf("msg to server: %s",message.msg); //print msg for error checking 

        //Take first character of message to get proper output 
        //Quit
        if (message.msg[0] == '4'){
            break;
        }
        //Domain name
        else if (message.msg[0] == '1') {
                getdomainname(temp, 256);
                sprintf(output.msg, "Domain Name: %s", temp);
	    }
        //Hostname
	    else if (message.msg[0] == '2'){
                gethostname(temp, 256);
                sprintf(output.msg, "Hostname: %s", temp);
	    }
        //UNAME
    	else if (message.msg[0] == '3'){
                uname(&unameData);
                sprintf(output.msg, "UNAME: %s %s release %s", unameData.sysname, unameData.version, unameData.release);
        }
        //Invalid input
        else{
               sprintf(output.msg, "%s", "Invalid request from client");
        }

        //Send output back to client
        sent_value = mq_send(que_server, (char *)&output, Q_MSG_SIZE, 0); 
        //empty the messages to accept new input
        strcpy(message.msg, ""); 
        strcpy(output.msg, ""); 
        
    }

    //close and unlink queues
    mq_close(que_client);
    mq_close(que_server);
    mq_unlink(Q_C_NAME);
    mq_unlink(Q_S_NAME);

    return 0;
}

/**********************************************************************************
* int main()
* Author: Brandon Atwal
* Date: 01/25/2022
* Description: The client will be taking the input which is typed in the terminal
and send it to the server function and then prients the response from the server
function.
**********************************************************************************/
int main()
{
    //fork the program
    pid_t pid; 
    pid = fork();
    //child becomes client, parent becomes server
    switch(pid){
        case -1:
            perror("Error in Fork"); 
            break;
        case 0:
            client(); 
            break; 
        default:
            server();
        break;
    }
	
}



