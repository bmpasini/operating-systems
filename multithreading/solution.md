Run the program and explain the output.
=============

I will start the explanation with a description of how the program works.

The main function initializes a mutex and also creates a thread, in which the child1 function will run.

In sequence, it locks the mutex, increase the counter by 1, print its value and then it unlocks the mutex. All of this happens in an infinite loop.

Meanwhile, the child1 function starts running concurrently. It locks the same mutex, which, if already locked, will put the child1 function in blocked state, until the mutex is unlocked by the function that locked it at the first place, which, in this case is the main function. Then, the child1 function locks the mutex and goes to the ready queue, to be run by the processor. Then, it increases the counter by 1, prints its value and unlocks the mutex. All of this also happens in an infinite loop.

The same procedure happens in the main function if its mutex locks while child1 has it locked already, then the former has to wait for the latter to unlock the mutex, going through the steps explained above.

Therefore, the output is the counter number, which is printed one time by the main function, and the other time by the child1 funtion, alternating between them in each of their loops. This happens until the main function prints the number 27, when the program reaches a deadlock. The reason why this happens will be explained below.

Why do the print statements stop appearing after a certain point in the program? Explain.
=============

The print statements stop appearing, because the thread that runs child1 is exited before unlocking the mutex. Then, as the mutex will be locked forever, the main function cannot run, and the program reaches a deadlock.

When the counter is more than 25, the next time child1 runs (when the counter is at 27), there is a condition that tells the thread to exit. However, this condition is after the mutex lock and before the mutex unlock. And, when a thread is exited, it doesn't automatically unlocks the mutex. A way to fix that will be presented below.


Modify the program and write a correct version that fixes the problem that you just discovered. Explain how you fixed the program.
=============

One way to fix this problem is to exit the thread after unlocking the mutex. This has been done in the file that follows enclosed.

In this file, the main thread keeps counting and printing after the second thread is exited.