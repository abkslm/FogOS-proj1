#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "user/psh.h"

/**
* psh (pride shell) executable toggle
* run `psh` to toggle
*/

#define OPEN_MSG "🌈 psh enabled! happy pride! 🌈"

int main (int argc, char *argv) {

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

		printf("%s\n", OPEN_MSG);
		set_color();

	} else {
		read(fd, buf, 1);
		buf[1] = '\0';

		close(fd);

		// i wanted to do some bitwise stuff here (i was feeling special)
		// it just does an XOR to either turn it on or off
		// (if the number in the toggle file is 1 (on), it changes to 0 (off), and vice versa)

		int toggle = buf[0] - '0';

		toggle = toggle ^ 1;

		fd = open(filename, O_WRONLY);
		if (fd < 0) {
			printf("Error: failed to reopen psh_toggle\n");
			exit(1);
		}

		buf[0] = toggle + '0';
		write(fd, buf, 1);

		if (toggle == 1) {
			printf("%s\n", OPEN_MSG);
			set_color();
		} else {
		    printf("\033[0mpsh disabled. you really afraid of rainbows? :(\n", buf[0]);
		}


	}

	close(fd);
	exit(0);
	
}
