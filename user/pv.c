/**
 * Implementation of pipe viewer (pv) for xv6
 *
 * This program reads data from standard input and writes it to standard output,
 * while displaying progress information (bytes transferred and transfer rate)
 * on standard error.
 */
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

#define BUFFER_SIZE 512

/**
 * Main function for the pipe viewer
 *
 * Reads data from standard input in chunks, writes it to standard output,
 * and displays progress information on standard error.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return 0 on success, 1 on error
 */
int
main(int argc, char *argv[])
{
	char buffer[BUFFER_SIZE];
	int bytes_read;
	unsigned int total_bytes = 0;
	int start_time = uptime();


	while ((bytes_read = read(0, buffer, sizeof(buffer))) > 0) {
		if (write(1, buffer, bytes_read) != bytes_read) {
			fprintf(2, "pv: write error\n");
			exit(1);
		}

		total_bytes += bytes_read;

		int elapsed_ticks = uptime() - start_time;

		if (elapsed_ticks == 0) {
			elapsed_ticks = 1;
		}

		int rate = total_bytes / elapsed_ticks;

		fprintf(2, "\r%d bytes transferred, %d bytes/tick", total_bytes, rate);
		 
	}

	if (bytes_read < 0) {
		fprintf(2, "pv: read error\n");
		exit(1);
	}

	fprintf(2, "\n");
	exit(0);

}
