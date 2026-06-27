// Define a structure for the lock
typedef struct _lock_t {
    int flag;       // 0 if lock is available, 1 if locked
    int guard;      // To prevent concurrent guard lock 
    /*
      The guard ensures that only one thread at a time is actualy 
      modifying the lock structure (the flag and the queue)
    */
    queue_t *q;     // Queue to hold waiting threads
} lock_t;

// Initialize the lock
void lock_init(lock_t *m) {
    m->flag = 0;     // Lock is initially available
    m->guard = 0;    // Guard is initially free
    queue_init(m->q); // Initialize the queue for waiting threads
}

// Acquire the lock
void lock(lock_t *m) {
    while (TestAndSet(&m->guard, 1) == 1)
        ; // Acquire guard lock by spinning

    if (m->flag == 0) {
        m->flag = 1;   // Lock is acquired
        m->guard = 0;  // Release the guard lock
    } else {
        queue_add(m->q, gettid()); // Add current thread to the waiting queue
        m->guard = 0;              // Release the guard lock
        park();                     // Put the thread to sleep
        /*
          If we release the guard after calling park(), something bad will happen
          The current thread will go to sleep and the guard will be 1, thus other
          threads will be stuck in an infinite loop trying to acquire the guard.
          Even the thread that's actually holding the lock will not be able to 
          release it (it will be stuck in the infinite loop when accessing the 
	  unlock while loop, failing to acquire the guard lock).
        */
    }
}

// Release the lock
void unlock(lock_t *m) {
    while (TestAndSet(&m->guard, 1) == 1)
        ; // Acquire guard lock by spinning

    if (queue_empty(m->q))
        m->flag = 0; // Release the lock; no one wants it
    else
        unpark(queue_remove(m->q)); // Wake up the next waiting thread
    m->guard = 0; // Release the guard lock
    /*
	If we release the guard before unparking one of the waiting threads,
	the guard will be free as well as the lock (cuz flag is set to 0). 
	So, if a new thread is created before that the waiting thread is 
	woken up, the new thread will acquire the lock, and the ordering will 
	be useless in this case.
    */
}
