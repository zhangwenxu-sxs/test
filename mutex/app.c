#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int fd;
	int i, count = 1;

	if(argc < 2)
	{
		printf("please input param: \n");
		return -1;
	}

	fd = open("/dev/chrdev0", O_RDWR, 0666);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}

	for (i=0; i<5; i++)
	{
		printf("process %d : count = %d\n",atoi(argv[1]), count);
		count++;
		sleep(1);
	}

	close(fd);

	return 0;
}

