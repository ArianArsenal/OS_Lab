#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>



int main(){
    int fd[2],n;
    char buffer[100];
    pid_t p;
    pipe(fd); //creates a unidirectional pipe
    p = fork();

    if(p>0){
        printf("Parent Passing value to child\n");
        write(fd[1],"hello\n",6); //fd[1] write end
        wait(NULL);

        close(fd[1]);

    }
    else{
        printf("Child Printing recieved value\n"); //fd[0] read end
        n = read(fd[0], buffer, 100);
        write(1, buffer, n);

        close(fd[0]);
        
    }

    
    return 0;
}