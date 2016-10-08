/* 
 * Operating Systems   (2INCO)   Practical Assignment
 * Threaded Application
 *
 * Julia van Straaten (0924283)
 * STUDENT_NAME_2 (STUDENT_NR_2)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * ”Extra” steps can lead to higher marks because we want students to take the initiative. 
 * Extra steps can be, for example, in the form of measurements added to your code, a formal 
 * analysis of deadlock freeness etc.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>     // for usleep()
#include <time.h>       // for time()

#include "prime.h"

typedef unsigned long long  MY_TYPE;

// create a bitmask where bit at position n is set
#define BITMASK(n)          (((MY_TYPE) 1) << (n))

// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))

// set bit n in v
#define BIT_SET(v,n)        ((v) =  (v) |  BITMASK(n))

// clear bit n in v
#define BIT_CLEAR(v,n)      ((v) =  (v) & ~BITMASK(n))


/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}

/* Returns the buffer element */
int elt (int i) {
    return i/64;
}


/* Returns the bit digit */
int bit (int i) {
    return i%64;
}


/* Thread functioning */
static void * thread (void * arg) {
    int *   argk;
    int     k;
    
    argk = (int *) arg;
    k = *argk;
    free (arg);
    
    rsleep(30);
    
    printf("I am a working thread with i: %d\n", k);
    
    return (0);
}


int main (void)
{
    // TODO: start threads generate all primes between 2 and NROF_SIEVE and output the results
    // (see thread_malloc_free_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)

    
    /* Set all bits to 1 */
    int i;
    for (i = 0; i < ((NROF_SIEVE/64) + 1); i++) {
        buffer[i] = ~0;
    }

    /* Delete 0 and 1 as primes */
    BIT_CLEAR(buffer[0],0);
    BIT_CLEAR(buffer[0],1);
    
    pthread_t my_threads[NROF_SIEVE];

    /* Delete all non-primes according to Sieve */
    int j;
    for (i = 2; (i*i) < NROF_SIEVE; i++) {
        //pthread_create (&my_threads[i], NULL, my_mutex_thread, NULL);
        if (BIT_IS_SET(buffer[elt(i)],bit(i))) {
            for (j = (i*i); j < NROF_SIEVE; j = j + i) {
                BIT_CLEAR(buffer[elt(j)],bit(j));
            }
        }
    }

    /* Display all primes */
    for (i = 2; i < 100; i++) {
        if (BIT_IS_SET(buffer[elt(i)],bit(i))) {
            printf("%d\n", i);
        }
    }


    return (0);
}
 
