//
//  main.c
//  DiningPhilosophers
//
//  Created by Lucas Oliveira on 27/09/23.
//

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define PHILOS 5
#define DELAY 5000
#define FOOD 1000

void *philosopher_routine (void *id);
int grab_chopstick (int, int, char *);
void down_chopsticks (int, int, int);
int food_on_table ();

int food = FOOD;

pthread_mutex_t chopstick[PHILOS];
pthread_t philosophers[PHILOS];
pthread_mutex_t food_lock;
int sleep_seconds = 0;


int main (int argn, char **argv) {
    int i;

    if (argn == 2)
        sleep_seconds = atoi (argv[1]);
    
    pthread_mutex_init (&food_lock, NULL);
    for (i = 0; i < PHILOS; i++)
        pthread_mutex_init (&chopstick[i], NULL);
    for (i = 0; i < PHILOS; i++) {
        pthread_create (&philosophers[i], NULL, philosopher_routine, (void *)i);
    }
    for (i = 0; i < PHILOS; i++)
        pthread_join (philosophers[i], NULL);
    return 0;
}

void * philosopher_routine (void *num) {
    int id;
    int left_chopstick, right_chopstick;
    
    id = (int)num;
    printf ("Philosopher %d is done thinking and now ready to eat.\n", id);
    right_chopstick = id;
    left_chopstick = id + 1;
    
    /* Wrap around the chopsticks. */
    if (left_chopstick == PHILOS)
        left_chopstick = 0;
    
    while (food > 0) {
        
        if (id == 1)
            sleep (sleep_seconds);
        
        if (grab_chopstick (id, right_chopstick, "right")) {
            printf("Philosopher %d have tried to use chopstick %d but it's blocked\n", id, right_chopstick);
            continue;
        }
        if (grab_chopstick (id, left_chopstick, "left")) {
            pthread_mutex_unlock (&chopstick[right_chopstick]);
            printf("Philosopher %d have tried to use chopstick %d but it's blocked, so he down the chopstick %d\n", id, left_chopstick, right_chopstick);
            continue;
        }
        
        food_on_table ();
        
        printf ("Philosopher %d: eating using chopstick %d and %d \n", id, left_chopstick, right_chopstick);
        usleep (DELAY * (FOOD - food + 1));
        down_chopsticks (id, left_chopstick, right_chopstick);
    }
    
    printf ("Philosopher %d is done eating.\n", id);
    return (NULL);
}

int food_on_table () {
    int myfood;
    
    pthread_mutex_lock (&food_lock);
    if (food > 0) {
        food--;
    }
    myfood = food;
    pthread_mutex_unlock (&food_lock);
    return myfood;
}

int grab_chopstick (int philosopher, int c, char *hand) {
    int result = pthread_mutex_trylock(&chopstick[c]);
    if (result)
        printf ("Philosopher %d can't get the chopstick %d\n", philosopher, c);
    else
        printf ("Philosopher %d: got %s chopstick %d\n", philosopher, hand, c);
    return result;
}

void down_chopsticks (int id, int c1, int c2) {
    pthread_mutex_unlock (&chopstick[c1]);
    pthread_mutex_unlock (&chopstick[c2]);
    printf("Philosopher %d: stop eating and down chopsticks %d and %d\n", id, c1, c2);
}
