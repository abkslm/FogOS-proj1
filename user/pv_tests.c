#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BUFFER_SIZE 1024
#define TEST_FILE "test_input.txt"
#define OUTPUT_FILE "test_output.txt"


/**
* writes a test file given contents.
* 
* @param content What to write to file
*/
void
create_test_file(char *content)
{

	// delete file if exists
	struct stat st;

    if (stat(TEST_FILE, &st) == 0) {
    	printf("File exists, deleting file.\n");
    	if (unlink(TEST_FILE) < 0) {
    		printf("Failed to delete file.\n");
    		exit(1);
    	}
    } 

	// create if file dne
    int fd = open(TEST_FILE, O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("Failed to create test file\n");
        exit(1);
    }
    
    write(fd, content, strlen(content));
    close(fd);
}


/**
* checks if pv correctly transfers data from input to output.
*/
void
test_pv_data_transfer()
{
    char *test_content = "This is a test file for pv.\n";
    create_test_file(test_content);

    int pid = fork();
    if (pid == 0) { // child process
    
        // closes stdin/out and replaces them w the test files
        // (so pv doesn't try to read from terminal + print pv output)
        close(0);
        open(TEST_FILE, O_RDONLY);
        close(1);
        open(OUTPUT_FILE, O_CREATE | O_WRONLY);
        
		//  name and a null pointer to terminate the list
        char *argv[] = {"pv", 0};
        exec("pv", argv);	// replaces process w pv, so shouldnt reach exit
        exit(1);  
    } else { // parent process
        wait(0);
       
        int fd = open(OUTPUT_FILE, O_RDONLY);
        if (fd < 0) {
            printf("FAIL: Could not open output file\n");
            exit(1);
        }

		// reads output into buf
        char buf[BUFFER_SIZE];
        
        read(fd, buf, BUFFER_SIZE);
        close(fd);

		// makes sure the test was copied correctly
        if (strcmp(buf, test_content) != 0) {
            printf("FAIL: Output does not match input\nTest content: '%s'\nOutput: '%s'\n", test_content, buf);
            exit(1);
        }

        printf("PASS: pv data transfer test\n");
    }
}

/**
* runs pv and checks if it exits successfully. 
*/
void
test_pv_progress_output()
{
    char *test_content = "This is a longerrrrrrrrrrrrrrrrr test file for pv.\n";	//TODO why does this print but not ^
    create_test_file(test_content);

    int pid = fork();
    if (pid == 0) { // child
    	// replace std in w test file
        close(0);
        open(TEST_FILE, O_RDONLY);

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
