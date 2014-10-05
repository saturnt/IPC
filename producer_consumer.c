#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#define MAX_QUEUE_SIZE 128


struct dcmsg_queue {
    unsigned short r_lst;
    unsigned short w_lst;
    int size;
    int read;
    int write;
    int msg[MAX_QUEUE_SIZE];
};

struct dcmsg_queue q;
pthread_t producer_thr;
pthread_t consumer_thr;

inline void 
add_to_producer_queue(int item){
    	
     unsigned short w_lst = 0;
	     
		
        if ((q.w_lst + 1) != q.r_lst) {
	   w_lst = (q.w_lst + 1) % q.size;
		printf("producer produced %d\n", item);
  	   	q.msg[w_lst] = item;
	   	q.write ++;
	   	q.w_lst = w_lst;  
      	} 
}


void *producer(){
  
      int r = 0;
      int loop_end = 0;
      int i = 0;
		
      printf("Producer thread started\n\n");
      for(;;){	
 	srand(time(NULL));
	r = rand();
        loop_end = (r > 0) ? (r %16) : 0;

	for(i = 0; i < loop_end; i++){
	  add_to_producer_queue((i + 100)); 		   	
	}
	
	sleep(1);
      }
 	
      pthread_exit(0);	
}

void *
consumer(){
      printf("Consumer thread started\n\n"); 	
      for(;;){
	printf("In consumer thread\n");	
	int r = 0;
	int msg = 0;	
	while (q.r_lst != q.w_lst) {		
		r = (q.r_lst + 1) % q.size;
		msg = q.msg[r];
		printf("Consumer read %d\n",msg);         
		q.read ++;
            	q.r_lst = r;
        }
	sleep(1);
      }	
     pthread_exit(0);	
}

int main(){
    int i;
    memset((void *)&q, 0, sizeof(struct dcmsg_queue));
    q.size = MAX_QUEUE_SIZE;	
    if(pthread_create(&producer_thr, NULL, (void *)&producer, NULL)){
	fprintf(stderr, "Error creating the producer thread\n");	
    } else {
	printf("Created the producer thread\n");
    }
    if(pthread_create(&consumer_thr, NULL, (void *)&consumer, NULL)){
	fprintf(stderr, "Error creating the consumer thread\n");
    } else {
	printf("Created the consumer thread\n");
    }	    		
     				
    pthread_join(producer_thr, NULL);
    pthread_join(consumer_thr, NULL);
	
}
