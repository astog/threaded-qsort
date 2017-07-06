### Threaded Quicksort
This is one implementation of the threaded-quicksort that creates a thread tree, with each parent thread responsible for creation and closing of it's children threads.

### Algorithm
The quicksort algorithm itself is not changed. If the length of the array to sort is big enough (*defined > seg_size*) then a thread is created to sort it, else it is sorted in the same thread.

### Performace
Test was done with creating a randomized array, and sorting it 20 times to calculate the average. The times here in ms.

Test was done on xubuntu-xenial running on a i7 processor with turboboost disabled, and clock frequency locked. Hyperthreading was left on.

| Array Size | qsort | thsort | Ratio |
|:----------:|:-----:|:------:|:-----:|
| 1000 | 137.3 | 129.05 | 1.06 |
| 20000 | 3536.3 | 1810.55 | 1.95 |
| 100000 | 20042.5 | 7598.3 | 2.64 |
| 500000 | 111958.1 | 38927.45 | 2.88 |
| 1000000 | 239363.1 | 76807.7 | 3.12 |
