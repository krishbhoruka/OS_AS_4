#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>


pthread_mutex_t forks[5];
pthread_mutex_t bowls[2];
pthread_cond_t cond[5];

#define NUM_PHILOSOPHERS 5
#define NUM_BOWLS 2


void eating();
void thinking();
void* philosopher();
int left(int p);
int right(int p);


int main(){
    pthread_t philosophers[5];
    int philosopher_ids[5];

    // Initialize mutexes and conditional variables
    for (int i = 0; i < 5; i++) {
        pthread_mutex_init(&forks[i], NULL);
        pthread_cond_init(&cond[i], NULL);
    }

    for (int i = 0; i < NUM_BOWLS; i++) {
        pthread_mutex_init(&bowls[i], NULL);
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopher_ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &philosopher_ids[i]);
    }

    // Wait for philosopher threads to finish (this will never happen in this example)
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_join(philosophers[i], NULL);
    }

    // Clean up
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_mutex_destroy(&forks[i]);
        pthread_cond_destroy(&cond[i]);
    }

    for (int i = 0; i < NUM_BOWLS; ++i) {
        pthread_mutex_destroy(&bowls[i]);
    }

    return 0;
}


int left(int p) { 
    if(p!=4)
        return p; 
    return 0;        
}

int right(int p) { 
    if(p!=4)
        return (p + 1) % 5; 
    return 4;
}

void* philosopher(void* args) {
    int philosopher_id = *((int*)args);
    int left_fork = left(philosopher_id);
    int right_fork = right(philosopher_id);

    while (1) {
        thinking(philosopher_id);

        pthread_mutex_lock(&forks[left_fork]);
        printf("Philosopher %d picked up left fork\n", philosopher_id+1);

        pthread_mutex_lock(&forks[right_fork]);
        printf("Philosopher %d picked up right fork\n", philosopher_id+1);

        //int res1 = 
        

        if(pthread_mutex_trylock(&bowls[0]) == 0){
            //pthread_mutex_lock(&bowls[0]);
            printf("Philosopher %d picked up bowl 1\n", philosopher_id+1);
            eating(philosopher_id);
            pthread_mutex_unlock(&bowls[0]);
        }
        else{
            if(pthread_mutex_lock(&bowls[1]) == 0){
                printf("Philosopher %d picked up bowl 2\n", philosopher_id+1);
                eating(philosopher_id);
                pthread_mutex_unlock(&bowls[1]);
            }
        }
        pthread_mutex_unlock(&forks[left_fork]);
        pthread_mutex_unlock(&forks[right_fork]);
    
    }
}

// sleeping for 3 seconds for each of activity performed
void eating(int philosopher_id) {
    printf("Philosopher %d is eating\n", philosopher_id+1);
    sleep(1); 
}

void thinking(int philosopher_id) {
    printf("Philosopher %d is thinking\n", philosopher_id+1);
    sleep(1); 
}
