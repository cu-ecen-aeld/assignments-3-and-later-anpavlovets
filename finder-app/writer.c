#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	char writefile[200] = "";
	char writestr[255] = "";
	int fd;
	
	openlog(NULL, 0, LOG_USER);
	if (argc < 2)
	{
		syslog(LOG_ERR, "2 arguments needed\n");
		return 1;
	}
	else
	{
		strcpy(writefile, argv[1]);
		fd = open(writefile, O_CREAT | O_RDWR, 0666);
		if (fd == -1)
		{
			syslog(LOG_ERR, "Can't open file %s\n", writefile);
			return 1;
			
		}
		else
		{
			strcpy(writestr, argv[2]);
			write(fd, writestr, strlen(writestr));
			syslog(LOG_DEBUG, "Writing %s to %s\n", writestr, writefile);
			close(fd);
			return 0;
		}
	}
}
