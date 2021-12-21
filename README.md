# Matrix-Multiplication 

Matrix Multiplication using shared memory and multiple processes.
Two matricies, A and B, must have the respective dimensions j x n and n x k in order to multiply them to create a j x k product matrix.  Using multiple processes to do matrix multiplication will speed up our runtime in comparison to a sequential implementation.  Work done on n rows must be split as evenly as possible among k workers. Each worker is responsible for (roughly) the floor of n/k rows.  The k subprocesses (k) will make use of a semaphore to coordinate computation of the product matrix.
