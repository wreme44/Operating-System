
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include<sys/wait.h>
#include <stdlib.h>
/* Witchel Reme
   z23008285*/


int main() {

    int fd[2]; // declare int array for pipe to send /receive message  (Pipe file descriptors)
    int status, retvaltwo; // declare int variable to store status of child process
    if (pipe(fd)== -1){ // checking for errors in pipe created
        perror("pipe");
        return -1;
        }
  
   pid_t pid= fork();//declare pid  variable to relay pid info when using fork   (first fork)

    
    if (pid < 0) {
        fprintf(stderr, "ERROR: fork failed with code %d\n", errno);
        return 1;

    }
    else if (pid == 0) { //child process of 1st fork 
        close(fd[0]);  // close the read end source because it is not needed, we will be writing
        dup2(fd[1], STDOUT_FILENO); // redirect standard output(STDOUT) to the write end of the pipe
        close(fd[1]); //close  the  write end after dup2
        printf("CHILD: my pid=%d\n\n", getpid());
        printf("CHILD: Now starting new command with exec:\n\n");
  
        execlp("ls", "ls", "-l", NULL);  //replace process image with "ls"
        // If exelcp returns, it means it has failed
        fprintf(stderr, "CHILD: ERROR: exec failed with code %d\n", errno);
        return -2;
    }
    else { // Parent (Initial) process, pid is the child's PID:
        // pid > 0
        close(fd[1]);  //close the write end source in parent , we will be reading
       
        printf("\n\nPARENT (my pid=%d): child process created with pid=%d\n\n", getpid(), pid);

        // wait for child to end; get child's return error code:
          
        wait(&status);

         pid_t childpid2 = fork();

                {
                if (childpid2<0){//2nd fork fail
                        fprintf(stderr, "2nd fork failed with code %d\n", errno);   //printing file stream 
                        }

                else if(childpid2==0){//  2nd child
                        close(fd[1]);  //child 2 redirects standard read (closes write end of pipe)
                        dup2(fd[0], STDIN_FILENO);  //child 2 redirects standard in to read end of the pipe

 
                        close (fd[0]);
                        printf("2nd child: my pid is: %d \n \n\n", getpid());
                        execlp("sort","sort",  "-r", "-n", "-k5", NULL); //parent should do the same thing as a shell that runs the comman "ps -aux | sort -r n -5k" 
                        }
                else{// 2nd parent child
                        close (fd[1]);

                        printf("\n\n Parent (my pid=%d): child process with pid=%d\n\n", getpid(), childpid2);
                        
                      
                        wait(&retvaltwo);

                        }
                        close (fd[0]);
                        return 0;
                
      printf("PARENT (my pid=%d): child process with pid=%d returned with error code=%d\n\n", getpid(), childpid2, WEXITSTATUS(retvaltwo));
                }
    }
    printf("OUTSIDE IF: my pid=%d\n\n", getpid());
    return 0; // 0 is the "normal" return code in case of no errors
}

