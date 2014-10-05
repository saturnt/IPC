#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<string.h>
#include<sys/mman.h>
#define MAX_QUEUE_SIZE 128

struct dcmsg_queue {
    unsigned short r_lst;
    unsigned short w_lst;
    int size;
    int read;
    int write;
    int msg[MAX_QUEUE_SIZE];
};

inline void
add_to_producer_queue(struct dcmsg_queue *q, int item){

     unsigned short w_lst = 0;


        if ((q->w_lst + 1) != q->r_lst) {
           w_lst = (q->w_lst + 1) % q->size;
                printf("producer produced %d\n", item);
                q->msg[w_lst] = item;
                q->write ++;
                q->w_lst = w_lst;
        }
}


int main(){

 struct dcmsg_queue *q; 
 
 sem_t *mutex;
 void *ptr = mmap(NULL, 
		 (sizeof(struct dcmsg_queue) + sizeof(sem_t)),
		 PROT_READ | PROT_WRITE,
		 MAP_SHARED | MAP_ANONYMOUS,
		 -1, 0	
		);  
  q = ptr;
  memset((void *)q, 0, sizeof(struct dcmsg_queue));
 
 q->size = MAX_QUEUE_SIZE;

  mutex = (ptr + sizeof(struct dcmsg_queue));
  if(sem_init(mutex, 1, 0)){
	perror("sem_init");
	return 1;	
  }

  int pid;
  pid = fork();	
  if(pid > 0){
  // This is the parent process.	

      int r = 0;
      int loop_end = 0;
      int i = 0;

      printf("Producer thread started\n\n");

      for(;;){
        srand(time(NULL));
        r = rand();
        loop_end = (r > 0) ? (r %16) : 0;

        for(i = 0; i < loop_end; i++){
          add_to_producer_queue(q, (i + 100));
        }
	
	sem_post(mutex);
        usleep(1);
	sem_wait(mutex);	
      }

  	

  } else if(pid == 0) {
  // This is the child process.	
      printf("Consumer thread started\n\n");

      for(;;){

	sem_wait(mutex);
        int r = 0;
        int msg = 0;
        while (q->r_lst != q->w_lst) {
                r = (q->r_lst + 1) % q->size;
                msg = q->msg[r];
                printf("Consumer read %d\n",msg);
                q->read ++;
                q->r_lst = r;
        }
	sem_post(mutex);
        usleep(1);
	
      }
	

  } else if(pid == -1){
	perror("Fork error");
  }
}
