#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PASSENGERS 100

// Global variables
int total_passengers;
int capacity;
int passengers_on_board = 0;
sem_t mutex, boardQueue, rideInProgress, unboardQueue;

void load() {
sem_wait(&mutex);
printf("Car: Loading passengers...\n");
sleep(1); // Simulating loading time
sem_post(&mutex);
}

void unload() {
sem_wait(&mutex);
printf("Car: Unloading passengers...\n");
sleep(1); // Simulating unloading time
sem_post(&mutex);
}

void board(int passenger_id) {
sem_wait(&mutex);
printf("Passenger %d: Boarding the car...\n", passenger_id);
sleep(1); // Simulating boarding time
sem_post(&mutex);
}

void offboard(int passenger_id) {
sem_wait(&mutex);
printf("Passenger %d: Getting off the car...\n", passenger_id);
sleep(1); // Simulating unboarding time
sem_post(&mutex);
}


// Car thread function
void* car(void* args) {
while (1) {
// Load passengers
load();

// Signal passengers to board
for (int i = 0; i < capacity; ++i) {
sem_post(&boardQueue);
}

// Wait for all passengers to board
for (int i = 0; i < capacity; ++i) {
sem_wait(&rideInProgress);
}

// Simulate ride duration
printf("Car: Ride in progress...\n");
sleep(2); // Simulating ride duration

// Unload passengers
unload();

// Signal passengers to unboard
for (int i = 0; i < capacity; ++i) {
sem_post(&unboardQueue);
}

// Wait for all passengers to unboard
for (int i = 0; i < capacity; ++i) {
sem_wait(&rideInProgress);
}
}

return NULL;
}

// Passenger thread function
void* passenger(void* args) {
int passenger_id = *((int*)args);

while (1) {
// Wait for the car to be ready for boarding
sem_wait(&boardQueue);

// Board the car
board(passenger_id);

// Signal car that passenger has boarded
sem_post(&rideInProgress);

// Wait for the car to finish the ride
sem_wait(&unboardQueue);

// Unboard the car
offboard(passenger_id);

// Signal car that passenger has unboarded
sem_post(&rideInProgress);
}

return NULL;
}

int main() {
// Input: Total passengers and car capacity
printf("Enter total number of passengers: ");
scanf("%d", &total_passengers);

printf("Enter car capacity: ");
scanf("%d", &capacity);


if (total_passengers<=0 || capacity<0){
    perror("Invalid");
    return 1;
}
// Initialize semaphores
sem_init(&mutex, 0, 1);
sem_init(&boardQueue, 0, 0);
sem_init(&rideInProgress, 0, 0);
sem_init(&unboardQueue, 0, 0);

// Create car thread
pthread_t car_thread;
pthread_create(&car_thread, NULL, car, NULL);

// Create passenger threads
pthread_t passenger_threads[MAX_PASSENGERS];
int passenger_ids[MAX_PASSENGERS];

for (int i = 0; i < total_passengers; ++i) {
passenger_ids[i] = i + 1;
pthread_create(&passenger_threads[i], NULL, passenger, &passenger_ids[i]);
}

// Join threads
pthread_join(car_thread, NULL);

for (int i = 0; i < total_passengers; ++i) {
pthread_join(passenger_threads[i], NULL);
}

// Destroy semaphores
sem_destroy(&mutex);
sem_destroy(&boardQueue);
sem_destroy(&rideInProgress);
sem_destroy(&unboardQueue);

return 0;
}

