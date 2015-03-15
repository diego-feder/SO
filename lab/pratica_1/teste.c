#include<stdio.h>
#include<unistd.h>
int main () {
	int fd=open("sh.c", 'r');
	printf("%d\n", fd);
	sleep(50);
	return 0;
}
