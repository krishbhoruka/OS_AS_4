#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CARS 1000

sem_t bridge_sem, mutex;
int left_count = 0, right_count = 0;

void passing(int dir, int car_number) {
    if (dir == 0) {
        sem_wait(&bridge_sem);
        printf("Car from left side with ID %d is crossing the bridge.\n", car_number);
        sleep(1);  
        printf("Car from left side with ID %d crossed the bridge.\n", car_number);
        sem_post(&bridge_sem);
    } else {
        sem_wait(&bridge_sem);
        printf("Car from right side with ID %d is crossing the bridge.\n", car_number);
        sleep(1);  
        printf("Car from right side with ID %d crossed the bridge.\n", car_number);
        sem_post(&bridge_sem);
    }
}
void* left(void* args) {
    int car_number = *((int*)args);

    sem_wait(&mutex);
    left_count++;
    sem_post(&mutex);

    passing(0, car_number);

    sem_wait(&mutex);
    left_count--;
    if (left_count == 0 && right_count > 0) {
        sem_post(&bridge_sem);
    }
    sem_post(&mutex);

    pthread_exit(NULL);
}

void* right(void* args) {
    int car_number = *((int*)args);

    sem_wait(&mutex);
    right_count++;
    sem_post(&mutex);

    passing(1, car_number);

    sem_wait(&mutex);
    right_count--;
    if (right_count == 0 && left_count > 0) {
        sem_post(&bridge_sem);
    }
    sem_post(&mutex);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[MAX_CARS * 2];
    sem_init(&bridge_sem, 0, 5);  
    sem_init(&mutex, 0, 1);

    int car_numbers[2*MAX_CARS];

    printf("Enter the number of cars on the left side: ");
    int left_cars;
    scanf("%d", &left_cars);
    printf("Enter the number of cars on the right side: ");
    int right_cars;
    scanf("%d", &right_cars);


    if (left_cars+right_cars>MAX_CARS){
        perror("Invalid");
        return 1;
    }
    if (left_cars<0 && right_cars<0){
        perror("Invalid");
        return 1;
    }
    for (int i = 0; i < left_cars; ++i) {
        car_numbers[i] = i + 1;
        pthread_create(&threads[i], NULL, left, &car_numbers[i]);
    }
    for (int i = 0; i < left_cars ; ++i) {
        pthread_join(threads[i], NULL);
    }
    for (int i = 0; i < right_cars; ++i) {
        car_numbers[i + left_cars] = i + 1;
        pthread_create(&threads[i + left_cars], NULL, right, &car_numbers[i + left_cars]);
    }

    for (int i = 0; i < right_cars ; ++i) {
        pthread_join(threads[i + left_cars], NULL);
    }

    sem_destroy(&bridge_sem);
    sem_destroy(&mutex);

    return 0;
}
