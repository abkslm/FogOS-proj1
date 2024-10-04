#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BUFFER_SIZE 1024
//updated to use a long file "time-machine.txt" as test input
#define TEST_FILE "time-machine.txt" 
#define OUTPUT_FILE "test_output.txt"

/**
* checks if pv correctly transfers data from input to output.
*/
void
test_pv_data_transfer()
{
    int pid = fork();
    if (pid == 0) { // child process
    
        // closes stdin/out and replaces them w the test files
        // (so pv doesn't try to read from terminal + print pv output)
        close(0);
        open(TEST_FILE, O_RDONLY); //time-machine.txt
        close(1);
        open(OUTPUT_FILE, O_CREATE | O_WRONLY);
        
		//  name and a null pointer to terminate the list
        char *argv[] = {"pv", 0};
        exec("pv", argv);	// replaces process w pv, so shouldnt reach exit
        exit(1);  
    } else { // parent process
        wait(0);
       
     wait(0);

        // Open both input (time-machine.txt) and output (test_output.txt) files
        int input_fd = open(TEST_FILE, O_RDONLY);
        if (input_fd < 0) {
            printf("FAIL: Could not open input file\n");
            exit(1);
        }

        int output_fd = open(OUTPUT_FILE, O_RDONLY);
        if (output_fd < 0) {
            printf("FAIL: Could not open output file\n");
            exit(1);
        }

        char input_buf[BUFFER_SIZE];
        char output_buf[BUFFER_SIZE];
        int input_bytes, output_bytes;

        // compate contents chunk by chunk
        while ((input_bytes = read(input_fd, input_buf, BUFFER_SIZE)) > 0) {
            output_bytes = read(output_fd, output_buf, BUFFER_SIZE);

            //if num  bytes doesn't match, fail 
            if (input_bytes != output_bytes) {
                printf("FAIL: Input and output file sizes differ\n");
                exit(1);
            }

            // if contents don't match, fail 
            if (memcmp(input_buf, output_buf, input_bytes) != 0) {
                printf("FAIL: Input and output file contents differ\n");
                exit(1);
            }
        }

        //check if the end of output also reached
        if ((output_bytes = read(output_fd, output_buf, BUFFER_SIZE)) > 0) {
            printf("FAIL: Output file is longer than input file\n");
            exit(1);
        }

        close(input_fd);
        close(output_fd);

        printf("PASS: pv data transfer test\n");
    }
}

/**
* runs pv and checks if it exits successfully. 
*/
void
test_pv_progress_output()
{

    int pid = fork();
    if (pid == 0) { // child
    	// replace std in w/ test file
        close(0);
        open(TEST_FILE, O_RDONLY); //time-machine.txt

        // run pv on test file
        char *argv[] = {"pv", 0};
        exec("pv", argv);
        exit(1);  
    } else { // parent 
    	// child's exit info
        int status;
        wait(&status);
 
		//extracts exit code stored in wait (I looked this up lol)
        if ((status & 0xff) != 0) {
            printf("FAIL: pv exited with non-zero status\n");
            exit(1);
        }
		// ! doesn't distinguish between normal non-0 exits and errors !
        printf("PASS: pv progress output test\n");
    }
}

int
main()
{
    printf("Running pv tests...\n");

    test_pv_data_transfer();
    test_pv_progress_output();

    printf("All tests completed!\n");
    exit(0);
}
