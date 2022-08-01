#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sched.h>

#define PACKET_LEN 1024
#define CLIENTS_NUM 10

int signal_received = 0;

void sig_handler(int signo)
{
	if ((signo == SIGINT) || (signo == SIGTERM))
	{
		signal_received = 1;
	}	
}

int main(int argc, char* argv[])
{	
	int listen_fd, client_fd;	
	char socket_port[] = "9000";
	struct addrinfo hints;
	struct addrinfo *servinfo, *rp;
	int status;
	char client_ip[40];//IPv6 - 39 symbols
	struct sockaddr_in client_addr;
	size_t addr_len = sizeof(client_addr);
	char socket_data_path[] = "/var/tmp/aesdsocketdata";
	int fd;
	char buf[PACKET_LEN];
	int bytes_read;
	int end_of_packet = 0;
	struct sigaction sig_struct;
	pid_t pid;
	int d_mode = 0;

// b. Opens a stream socket bound to port 9000, failing and returning -1 if any of the socket connection steps fail.
	
	/*memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;*/
	memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
        hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        hints.ai_protocol = 0;          /* Any protocol */
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;
	
	memset(&sig_struct, 0, sizeof(sig_struct));
	sig_struct.sa_handler = sig_handler;
	
	if ( (status = getaddrinfo(NULL, socket_port, &hints, &servinfo)) != 0)
	{
		perror("getaddrinfo error\n");
		return -1;
	}

	/*listen_fd = socket(AF_INET, hints.ai_socktype, hints.ai_protocol);
    	if (listen_fd == -1)
    	{
       		perror("socket error");
       		freeaddrinfo(servinfo);
       		return -1;
    	}

	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	    perror("setsockopt(SO_REUSEADDR) failed");
	    
    	if ((bind(listen_fd, servinfo->ai_addr, sizeof(struct sockaddr))) != 0)
    	{
       		perror("bind error");
       		close(listen_fd);
       		freeaddrinfo(servinfo);
       		return -1;
    	}*/
    
    	for (rp = servinfo; rp != NULL; rp = rp->ai_next) 
    	{
     		listen_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
               	if (listen_fd == -1)
                   continue;
		if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		    perror("setsockopt(SO_REUSEADDR) failed");
               	if (bind(listen_fd, rp->ai_addr, rp->ai_addrlen) == 0)
                   break;                  /* Success */

               close(listen_fd);
        }    

    freeaddrinfo(servinfo);
    openlog(NULL, 0, LOG_USER);
        if ( (argc == 2) && (!(strcmp(argv[1], "-d")) ) )//make a daemon
        {
        	d_mode = 1;
        	syslog(LOG_DEBUG, "I'm daemon");
        }
        if (d_mode)        
        	pid = fork();
        if (d_mode)
        {        	
        	if (pid == 0)
        	{
        		syslog(LOG_DEBUG, "I'm child");
        		setsid();
        		chdir("/");
        	}
        	else if (pid > 0)
        	{
        		syslog(LOG_DEBUG, "Exit parent");
        		close(listen_fd);
        		return 0;
        	}
        	else 
        	{
        		printf("bad fork\n");
        		return -1;
        	}
        }       
       
        sigaction(SIGINT, &sig_struct, NULL);
        sigaction(SIGTERM, &sig_struct, NULL);
        fd = open(socket_data_path, O_RDWR | O_CREAT | O_APPEND, 0666);
// c. Listens for and accepts a connection        
	listen(listen_fd, CLIENTS_NUM);	        
	syslog(LOG_DEBUG, "listen");
	while (!signal_received)
	{
// h. Restarts accepting connections from new clients forever in a loop until SIGINT or SIGTERM is received (see below).
		memset(&client_addr, 0, sizeof(client_addr));
		client_fd = accept(listen_fd, (struct sockaddr*) &client_addr, (socklen_t *)&addr_len);
		if ((client_fd < 0) && (errno == EINTR)) //accept interrupted by signal
		{			
			break;
		}	
		else if (client_fd < 0)
		{
			syslog(LOG_DEBUG, "accept_error");
			perror("accept_error:");
			close(listen_fd);
			return -1;	
			
		}
	        syslog(LOG_DEBUG, "accepted");		
		strcpy(client_ip, inet_ntoa(client_addr.sin_addr));
		
// d. Logs message to the syslog “Accepted connection from xxx” where XXXX is the IP address of the connected client. 	
		syslog(LOG_DEBUG, "Accepted connection from %s\n", client_ip);
	
/*	 e. Receives data over the connection and appends to file /var/tmp/aesdsocketdata, creating this file if it doesn’t exist.

Your implementation should use a newline to separate data packets received.  In other words a packet is considered complete when a newline character is found in the input receive stream, and each newline should result in an append to the /var/tmp/aesdsocketdata file.

You may assume the data stream does not include null characters (therefore can be processed using string handling functions).

You may assume the length of the packet will be shorter than the available heap size.  In other words, as long as you handle malloc() associated failures with error messages you may discard associated over-length packets.
*/	
		
		while (!end_of_packet)
		{
			bytes_read = read(client_fd, buf, PACKET_LEN);
			if (bytes_read == -1)
			{
				close(fd);
				close(client_fd);
				close(listen_fd);
				return -1;
			}
			if (buf[bytes_read - 1] == '\n')					
				end_of_packet = 1;
			write(fd, buf, bytes_read);
			
		}		
		end_of_packet = 0;	
/*	  f. Returns the full content of /var/tmp/aesdsocketdata to the client as soon as the received data packet completes.

You may assume the total size of all packets sent (and therefore size of /var/tmp/aesdsocketdata) will be less than the size of the root filesystem, however you may not assume this total size of all packets sent will be less than the size of the available RAM for the process heap.
*/
//		printf("bytes_read=%d\n", bytes_read);
		lseek(fd, 0, SEEK_SET);	
		while (bytes_read > 0)
		{
			bytes_read = read(fd, buf, PACKET_LEN);
			write(client_fd, buf, bytes_read);
		}
		lseek(fd, 0, SEEK_END);	
		

	// g. Logs message to the syslog “Closed connection from XXX” where XXX is the IP address of the connected client.
		syslog(LOG_DEBUG, "Closed connection from %s\n", client_ip);
		close(client_fd);
	}//end of while (!signal_received)
	
	//signal received; close sockets
	syslog(LOG_DEBUG, "Caught signal, exiting\n");
	if (client_fd >= 0)
		close(client_fd);
	close(listen_fd);
	close(fd);
	status = remove(socket_data_path);		
	if (status != 0)
		syslog(LOG_DEBUG, "file remove error");
	
	return 0;
}
