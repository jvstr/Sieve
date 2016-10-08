/* 
 * Operating Systems   (2INCO)   Practical Assignment
 * Threaded Application
 *
 * Julia van Straaten (0924283)
 * Rolf Verschuuren (0916476)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * ”Extra” steps can lead to higher marks because we want students to take the initiative. 
 * Extra steps can be, for example, in the form of measurements added to your code, a formal 
 * analysis of deadlock freeness etc.
 */
 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>     // for usleep()
#include <time.h>       // for time()
#include <pthread.h>

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

// declare and initialize the mutex locks
static pthread_mutex_t      bufferlock          = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t      nrthreadlock          = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t      threadcreationlock          = PTHREAD_MUTEX_INITIALIZER;

// record the number of threads in use
int nrThreads = 0;


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

/* Returns the id of the buffer that contains the given number */
int elt (int i) {
    return i/64;
}


/* Returns the position of the given number in the buffer */
int bit (int i) {
    return i%64;
}


/* Thread functioning */
static void * thread (void * arg) {
    int *   argk;
    int     k;
    
    // deal with the argument
    argk = (int *) arg;
    k = *argk;
    free (arg);
    
    // remove multiples of the argument from the buffer (they are not primes)
    int j;
    for (j = (k*k); j < NROF_SIEVE; j = j + k) {
        // random sleep to make the execution order less predictable
        rsleep(100);
        
        // critical region
        pthread_mutex_lock(&bufferlock);
        BIT_CLEAR(buffer[elt(j)],bit(j));
        pthread_mutex_unlock(&bufferlock);
        // end critical region
    }
    
    // decrement nr of threads in use
    pthread_mutex_lock(&nrthreadlock);
    nrThreads--;
    pthread_mutex_unlock(&nrthreadlock);
    
    // allow another thread to be created
    pthread_mutex_unlock(&threadcreationlock);
    
    // exit
    return (0);
}


int main (void)
{
    // start threads generate all primes between 2 and NROF_SIEVE and output the results
    // (see thread_malloc_free_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)

    
    /* Set all bits in the buffer to 1 */
    int i;
    for (i = 0; i < ((NROF_SIEVE/64) + 1); i++) {
        buffer[i] = ~0;
    }

    /* Delete 0 and 1 as primes */
    BIT_CLEAR(buffer[0],0);
    BIT_CLEAR(buffer[0],1);
    
    // Array that holds potential thread IDs
    pthread_t my_threads[((int) sqrt(NROF_SIEVE))];

    /* Delete all non-primes according to Sieve */
    for (i = 2; (i*i) < NROF_SIEVE; i++) {
        // check whether the number is not yet striped out (possibly a prime)
        if (BIT_IS_SET(buffer[elt(i)],bit(i))) {
            
            //create parameter value for a new thread
            int * parameter;
            parameter = malloc (sizeof (int));
            *parameter = i;
            
            // create a new thread
            pthread_create (&my_threads[i], NULL, thread, parameter);
            
            // increment number of threads in use
            pthread_mutex_lock(&nrthreadlock);
            nrThreads++;
            pthread_mutex_unlock(&nrthreadlock);
        }
        
        // wait until there are fewer than NROF_THREADS threads
        while (nrThreads >= NROF_THREADS) {
            pthread_mutex_lock(&threadcreationlock);
        }
    }
    
    // Wait for the threads to close
    while (nrThreads > 0) {
        pthread_mutex_lock(&threadcreationlock);
    }
    
    /* Display all primes */
    for (i = 2; i < NROF_SIEVE; i++) {
        if (BIT_IS_SET(buffer[elt(i)],bit(i))) {
            printf("%d\n", i);
        }
    }


    return (0);
}
 
