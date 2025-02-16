#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char* NLP(char* inp){
    char* ans;
    int i=0;
    while(inp[i] != NULL){
        if((int)inp[i] >= 65 && (int)inp[i] <= 90)
            ans[i]= inp[i]+ 32;
        else if((int)inp[i] >= 97 && (int)inp[i] <= 122)
            ans[i]= inp[i]- 32;
        else
            ans[i]= inp[i];
        i ++;
    }
    ans[i]= '\0';
    return ans;
}

int main(){
    int fd[2], n;
    char buf[100];
    int pid;
    pipe(fd);
    pid = fork();
    if(pid){
        write(fd[1], "This Is My txt KOMAK!", 21);
        wait(0);
        close(fd[1]);
    }

    else{
        n= read(fd[0], buf, 21);
        write(1, buf, 21);
        char* ans = NLP(buf);
        printf("\n%s\n", ans);
        close(fd[0]);
    }


    return 0;
}