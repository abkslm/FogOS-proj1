#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/fcntl.h"

/**
* psh (pride shell)
* run `psh` to toggle
*/

int
main(int argc, char *argv)
{
	char *filename = "/tmp/psh_toggle";
	int fd;
	char buf[2];

	fd = open(filename, O_RDWR);

	if (fd < 0) {
		fd = open(filename, O_CREATE | O_RDWR);
		if (fd < 0) {
			printf("Error: failed to create psh_toggle file.\n");
			exit(1);
		}
		buf[0] = '1';
		buf[1] = '\0';
		write(fd, buf, 1);
		printf("psh enabled! happy pride!\n");

	} else {
		read(fd, buf, 1);
		buf[1] = '\0';

		close(fd);

		int bit = buf[0] - '0';

		bit = bit ^ 1;

		fd = open(filename, O_WRONLY);
		if (fd < 0) {
			printf("Error: failed to reopen psh_toggle\n");
			exit(1);
		}

		buf[0] = bit + '0';
		write(fd, buf, 1);

		if (bit == 1) {
			printf("psh enabled! happy pride!\n");
		} else {
		    printf("\033[0mpsh disabled. you really afraid of rainbows? :(\n", buf[0]);
		}


	}

	close(fd);
	exit(0);
}
