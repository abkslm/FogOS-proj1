#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "user/psh.h"

//
//	psh accessory functions for sh.c (keeps things clean)
//


// 8-Bit color array
char *colors_256[] = {

    "\033[38;5;196m",  // Red
    "\033[38;5;202m",  // Light Red/Orange
    "\033[38;5;208m",  // Orange
    "\033[38;5;214m",  // Light Orange
    "\033[38;5;226m",  // Yellow
    "\033[38;5;190m",  // Light Yellow/Green
    "\033[38;5;46m",   // Green
    "\033[38;5;83m",   // Light Green
    "\033[38;5;51m",   // Cyan
    "\033[38;5;81m",   // Light Cyan
    "\033[38;5;111m",  // Sky Blue (Lighter Blue)
    "\033[38;5;69m",   // Teal/Light Blue
    "\033[38;5;33m",   // Dark Blue
    "\033[38;5;201m",  // Magenta
    "\033[38;5;207m",  // Light Magenta
	
};


/*
*
* read_line_count() -> returns the current line number for color choice
*
*/
int
read_line_count()
{
	int fd, line_count = 0;
	fd = open("/tmp/line_count", O_RDONLY);
	if (fd >= 0) {
		read(fd, &line_count, sizeof(line_count));
		close(fd);
	}
	return line_count;
}

/*
*
* save_line_count(int line_count) -> takes the next line count and saves it to the counter file
*
*/
void
save_line_count(int line_count)
{
	int fd = open("/tmp/line_count", O_WRONLY | O_CREATE);
	write(fd, &line_count, sizeof(line_count));
	close(fd);
}

/*
*
* is_psh_enabled() -> checks if psh is enabled (by calling `psh`), returns 1 if enabled, 0 if not 
*
*/

int
is_psh_enabled()
{
	int fd;
	char buf[2];

	fd = open("/tmp/psh_toggle", O_RDONLY);

	if (fd < 0) {
		return 0;
	}

	read(fd, buf, 1);
	buf[1] = '\0';

	close(fd);

	return (buf[0] == '1') ? 1 : 0;
}


/*
*
* reset_color() -> resets the terminal color to white
*
*/

void
reset_color()
{
	printf("\033[0m");
}



/*
*
* set_next_color() -> moves color to next in `colors_256[]`, updates current line number
*
*/
void increment_color() {
	if (is_psh_enabled()) {
		set_color();
	} else {
		reset_color();
	}
}

void set_color() {
	int line_count = read_line_count();
	printf("%s", colors_256[line_count % (sizeof(colors_256) / sizeof(colors_256[0]) - 1)]);
	line_count++;
	save_line_count(line_count);
}


