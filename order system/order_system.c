#include "order_system.h"

pthread_mutex_t lock_cooks; //mutex for the cooks
pthread_cond_t  cond_cooks; //condition variable for cooks

pthread_mutex_t lock_ovens; //mutex for the ovens
pthread_cond_t  cond_ovens; //condition variable for ovens

pthread_mutex_t lock_deliverers; //mutex for the deliverers
pthread_cond_t  cond_deliverers; //condition variable for deliverers

pthread_mutex_t screen_mutex; //mutex for screen

int Ncook=2;  //number of available cooks
int Noven=5;  //number of available ovens
int Ndeliverer=10;  //number of available deliverers
long secondsX,secondsY; //time needed for each order
long sumX,sumY=0,maxX,maxY=-1;
int Npizza; //number of pizzas
int t_deliver; //time needed to deliver an order

void *order(void *x){

    int id = *(int *)x;
    int rc;
    int t_prepare=Npizza*Tprep; //preparation time for each order

    struct timespec start_threadX, finish_threadX; //variables for start and finish
                                                  //total time of each order
    struct timespec start_threadY, finish_threadY; //variables for start and finish
                                                 //time after the pizzas where baked
    
    
    clock_gettime(CLOCK_REALTIME, &start_threadX); //start total time

    rc = pthread_mutex_lock(&lock_cooks); //lock mutex for cooks
    if (rc != 0) {	
	printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
	pthread_exit(&rc);
    }	

    while (Ncook == 0) { //wait if there are no available cooks
	rc = pthread_cond_wait(&cond_cooks, &lock_cooks);
 	if (rc != 0) {	
		printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
		pthread_exit(&rc);
	}
    }

    Ncook--; //dicrease the number of available cooks
    rc = pthread_mutex_unlock(&lock_cooks); //unlock mutex for cooks
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
    }	
    
    sleep(t_prepare); //prepare the pizzas
 
    rc = pthread_mutex_lock(&lock_ovens); //lock mutex for ovens	
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
    }

    while (Noven == 0) { //wait until there is an available oven
	rc = pthread_cond_wait(&cond_ovens, &lock_ovens);
        if (rc != 0) {	
		printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
		pthread_exit(&rc);
	}
    }

    Noven--; //dicrease the number of available ovens 

    rc = pthread_mutex_unlock(&lock_ovens); //unlock mutex for ovens
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
    }	
    rc = pthread_mutex_lock(&lock_cooks); //lock mutex for cooks
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
    }
    Ncook++; //increase the number of available cooks
    rc = pthread_cond_signal(&cond_cooks); // condition signal for the other orders
    if (rc != 0) {	
	        printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
		pthread_exit(&rc);
    }
    rc = pthread_mutex_unlock(&lock_cooks); //unlock mutex for cooks
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
    }

    sleep(Tbake); //bake pizzas
    
    clock_gettime(CLOCK_REALTIME, &start_threadY);//calculate time needed for an
                                                  //order to get cold
    rc = pthread_mutex_lock(&lock_deliverers); //lock mutex for deliverers	
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
    }

    while (Ndeliverer == 0) { //wait until there is an available deliverer
	rc = pthread_cond_wait(&cond_deliverers, &lock_deliverers);
        if (rc != 0) {	
		printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
		pthread_exit(&rc);
	}
    }

    Ndeliverer--; //dicrease the number of available deliverers

    rc = pthread_mutex_unlock(&lock_deliverers); //unlock mutex for deliverers
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
    }	

    
    rc=pthread_mutex_lock(&lock_ovens); //lock mutex for ovens
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
    }

    Noven++; //increase the number of available ovens
    rc = pthread_cond_signal(&cond_ovens); //condition signal for the other orders
    if (rc != 0) {	
	        printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
		pthread_exit(&rc);
    }	

    rc = pthread_mutex_unlock(&lock_ovens); //unlock mutex for ovens
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
    }	
    
    sleep(t_deliver); //time needed for delivery
    clock_gettime(CLOCK_REALTIME, &finish_threadX); //finish total time
 
    clock_gettime(CLOCK_REALTIME, &finish_threadY);//finish time for an order to
                                                   //get cold
    
    sleep(t_deliver);//time needed for deliverer to get back to the restaurant
    
    rc=pthread_mutex_lock(&lock_deliverers); //lock mutex for deliverers
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
    }

    Ndeliverer++; //increase the number of available deliverers
    rc = pthread_cond_signal(&cond_deliverers); //condition signal for the other
                                                //orders
    if (rc != 0) {	
	        printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
		pthread_exit(&rc);
    }	

    rc = pthread_mutex_unlock(&lock_deliverers); //unlock mutex for deliverers
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
    }
    	
    rc = pthread_mutex_lock(&screen_mutex);//lock mutex for screen in order to
                                          // print the results
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
    }

    secondsX = finish_threadX.tv_sec - start_threadX.tv_sec; //calculate total time
                                                            //needed for each order
    secondsY = finish_threadY.tv_sec - start_threadY.tv_sec;//calculate time
                                                           //needed for each order
                                                          //to get cold
    sumX+=secondsX; //total time needed for all orders
    if(secondsX>maxX)
	maxX=secondsX; //find the maximum time of an order
    sumY+=secondsY; //total time needed for all orders to get cold
    if(secondsY>maxY)
	maxY=secondsY; //find the maximum time of an order to get cold
    printf("Order %d was ready in %ld minutes and was getting cold for %ld minutes \n ",id,secondsX,secondsY);
    rc = pthread_mutex_unlock(&screen_mutex); //unlock mutex for screen
    if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
    }

    pthread_exit(NULL);
}


int main(int argc, char** argv)  {

    int rc,Ncust;
    if (argc != 3) {
		printf("ERROR: the program should take 3 arguments!\n");
		exit(-1);
	}

	Ncust= atoi(argv[1]); //number of customers
	long int seed=atoi(argv[2]);   //seed for rand_r

	if (Ncust<0 || seed<0) { //checks if given arguments were valid
		printf("ERROR: the numbers should be positive");
		exit(-1);
	}
	
    pthread_t threads[Ncust]; //array for threads
    int id[Ncust]; //array for orders' id
    int t_create; //time for next order 

    rc=pthread_mutex_init(&screen_mutex, NULL);//initializes the mutex for screen
    if (rc != 0) {
    		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
       		exit(-1);
    }
    rc=pthread_mutex_init(&lock_cooks, NULL); //initializes the mutex for the cooks
    if (rc != 0) {
    		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
       		exit(-1);
    }
    rc=pthread_mutex_init(&lock_ovens, NULL); //initializes the mutex for the ovens
    if (rc != 0) {
    		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
       		exit(-1);
    }
    rc=pthread_mutex_init(&lock_deliverers, NULL); //initializes the mutex for the
                                                  //deliverers
    if (rc != 0) {
    		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
       		exit(-1);
    }
    rc=pthread_cond_init(&cond_cooks, NULL);  //initializes the condition variable
                                              // for the cooks
    if (rc != 0) {
    		printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
       		exit(-1);
    }
    rc=pthread_cond_init(&cond_ovens, NULL);  //initializes the condition variable
                                              //  for the ovens
    if (rc != 0) {
    		printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
       		exit(-1);
    }
    rc=pthread_cond_init(&cond_deliverers, NULL); //initializes the condition
                                                  //variable for the deliverers
    if (rc != 0) {
    		printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
       		exit(-1);
    }
    for (int i = 0; i < Ncust; i++) { //create threads 
	id[i] = i+1;
    	rc = pthread_create(&threads[i], NULL, order, &id[i]);
        if (rc != 0) {
    		printf("ERROR: return code from pthread_create() is %d\n", rc);
       		exit(-1);
	}
        unsigned int myseed = time(&seed) ^ getpid() ^ pthread_self();
        Npizza=rand_r(&myseed)%Norderhigh+Norderlow; //number of pizzas
        t_create=rand_r(&myseed)%Torderhigh+Torderlow; //time for next order
        t_deliver=rand_r(&myseed)%Thigh+Tlow; //time needed for delivery
	sleep(t_create); //time for next customer
    }

    for (int i = 0; i < Ncust; i++) { //join threads 
	rc=pthread_join(threads[i], NULL);
        if (rc != 0) {
		printf("ERROR: return code from pthread_join() is %d\n", rc);
		exit(-1);		
		}
    }
    
    rc=pthread_mutex_destroy(&screen_mutex); //destroy mutex for screen
    if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
    }
    rc=pthread_mutex_destroy(&lock_cooks);  //destroy mutex for cooks
    if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
    }
    rc=pthread_mutex_destroy(&lock_ovens); //destroy mutex for ovens
    if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
    }
    rc=pthread_mutex_destroy(&lock_deliverers); //destroy mutex for deliverers
    if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
    }
    rc=pthread_cond_destroy(&cond_cooks); //destroy condition variable for cooks
    if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
    }
    rc=pthread_cond_destroy(&cond_ovens); //destroy condition variable for ovens
    if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
    }
    rc=pthread_cond_destroy(&cond_deliverers); //destroy condition variable for
                                               //deliverers
    if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
    }
    printf("Maximum time for orders was %ld minutes\n",maxX);
    printf("Average time for orders was %ld minutes\n", sumX/Ncust);
    printf("Maximum time for a pizza to get cold was %ld minutes\n",maxY);
    printf("Average time for a pizza to get cold %ld minutes\n", sumY/Ncust);
    return 0;
}
