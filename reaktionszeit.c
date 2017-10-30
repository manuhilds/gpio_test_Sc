#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <poll.h>
// Change the sizeof with strlen !!

const char EXPORT_IN[] = "191";
const char EXPORT_OUT[]= "200";
const char DIR_IN[]	   = "in";
const char DIR_OUT[]	 = "out";
const char ON	   		   = '1';
const char OFF         = '0';
const char INT_TYPE[]  = "falling";

#define MAX         20
// Define Time for gettimeofday
struct timeval my_time,tmp;
void init_time();
// define file descriptor struct
struct fd {
	int fd_exports;
	int fd_unexports;
	int fd_dirin;
	int fd_dirout;
	int fd_valuein;
	int fd_valueout;
	int fd_edge; //interrupt
} init;
// define poll struct
struct pollfd poll_bouton;
void init_led(struct fd *init);
void los_led();
void interrupt_init();
int main(int argc,char *argv[])
{
	size_t random = 1,microsekunde;
  char buf = 0,ready;
	struct fd init;
	printf("bist du bereit?\n");
	init_led(&init);
	interrupt_init(&init);
	init_time();
	random = rand()%3+1;
	scanf("%c",&ready);
	usleep(random * 1e6);
	write(init.fd_valueout,(void*)&ON,sizeof(ON));
	gettimeofday(&tmp,NULL);
	lseek(init.fd_valuein, 0, SEEK_SET);
	read(init.fd_valuein,(void*)&buf,sizeof(buf));
	while(buf=='1'){
		lseek(init.fd_valuein, 0, SEEK_SET);
		read(init.fd_valuein,(void*)&buf,sizeof(buf));
		poll(&poll_bouton,1,10e3);
	}

	gettimeofday(&my_time,NULL);
	microsekunde=my_time.tv_usec-tmp.tv_usec;
	printf("Reaktionszeit:\t 0.%ds\n",microsekunde);
	write(init.fd_valueout,(void*)&OFF,sizeof(OFF));

	los_led(&init);
	return (EXIT_SUCCESS);
}
void  init_led(struct fd *init)
{
	//puts("0");
	if (open("/sys/class/gpio/export",O_WRONLY)==-1)																// configure
	{perror("error open export : ");																								//*
	 exit(EXIT_FAILURE);																														// the  input
 }
 																																//*
	else{
		//puts("0/1");																																				//*
		init->fd_exports= (open("/sys/class/gpio/export",O_WRONLY));
		//puts("0/2");								// ports
		if(write(init->fd_exports,(void*)(EXPORT_IN),strlen(EXPORT_IN))==-1)					//
			perror("error write export in : ");																					//
		if(write(init->fd_exports,(void*)(EXPORT_OUT),sizeof(EXPORT_OUT))==-1)
			perror("error write export out : ");
	}
	//puts("1");
	if(open("/sys/class/gpio/gpio200/direction",O_WRONLY)==-1)
	{perror("error open gpio200/direction : ");
		exit(EXIT_FAILURE);
	}
	else {
		init->fd_dirout=open("/sys/class/gpio/gpio200/direction",O_WRONLY);
		if(write(init->fd_dirout,(void*)(DIR_OUT),sizeof(DIR_OUT))==-1)
			perror("error write gpio200/direction : ");
	}
	//puts("2");
	if (open("/sys/class/gpio/gpio191/direction",O_WRONLY)==-1)
	{perror("error open gpio191/direction : ");
		exit(EXIT_FAILURE);
	}
	else{
		init->fd_dirin=open("/sys/class/gpio/gpio191/direction",O_WRONLY);
		if(write(init->fd_dirin,(void*)(DIR_IN),sizeof(DIR_IN))==-1)
			perror("error write gpio191/direction : ");
	}
	//puts("3");
	if (open("/sys/class/gpio/gpio200/value",O_RDWR)==-1)
	{perror("error open gpio200/value : ");
		exit(EXIT_FAILURE);
	}
	else
		init->fd_valueout=open("/sys/class/gpio/gpio200/value",O_RDWR);

	//puts("4");
	init->fd_valuein=open("/sys/class/gpio/gpio191/value",O_RDWR);
	if (init->fd_valuein==-1)
	{perror("error open gpio191/value : ");
		exit(EXIT_FAILURE);
	}

}

void los_led(struct fd* init)
{
	write(init->fd_valueout,(void*)&OFF,sizeof(OFF));
	if(open("/sys/class/gpio/unexport",O_WRONLY)==-1)
	{perror("error open unexport : ");
		exit(EXIT_FAILURE);
	}
	else{
		init->fd_unexports=open("/sys/class/gpio/unexport",O_WRONLY);
		write(init->fd_unexports,(void*)(EXPORT_OUT),sizeof(EXPORT_OUT));
		write(init->fd_unexports,(void*)(EXPORT_IN),sizeof(EXPORT_IN));
	}
	close (init->fd_exports);
	close (init->fd_unexports);
	close (init->fd_dirin);
	close (init->fd_dirout);
	close (init->fd_valuein);
	close (init->fd_valueout);
	close (init->fd_edge);
}

void init_time()
{if(gettimeofday(&my_time,NULL)<0)
		{perror("Time not given");
		exit(EXIT_FAILURE);}
	srand((unsigned int)my_time.tv_sec);
}

void interrupt_init(struct fd* init)
{
	init->fd_edge= open("/sys/class/gpio/gpio191/edge",O_RDWR);
	if (init->fd_edge<0)
	{perror("Error open gpio191/edge (interrupt): ");
		exit(EXIT_FAILURE);
	}
	if(write(init->fd_edge,(void*)INT_TYPE,sizeof(INT_TYPE))<0)
	{perror("Error write gpio191/edge (interrupt): ");
		exit(EXIT_FAILURE);
	}
	poll_bouton.fd = init->fd_valuein;
	poll_bouton.events =POLLPRI;
}
