My_Malloc Assignment
Andrew Fogarty - 260535895
=======================

Malloc
-------------------

The malloc code is located in the malloc directory.

My implementation follows the implementation from the assignment instructions pretty closely.

The main difference is that instead of using "tags" to tell if two free blocks are contiguous, I instead use the free
block linked list to check if a the previous or next block is contiguous with the current block.

In other words, when I free memory, I create a new free block and then put that free block at the appropriate place
in the free block linked list.  I then look at the the previous and next free blocks in the linked list.  By comparing
block sizes, I can tell if these blocks are contiguous.  If they are contiguous, then I merge them.

Another point to note is that when running my_malloc(x), I automatically add a few bytes (2 * void pointer size + 1) so
that we don't have fragmentation issues when that "unfree" block eventually gets freed and converted to a free block.

One final thing to note is that when I need to grow the size of the heap on my_malloc(x), I grow it by 2 * x + 64 bytes.
This way, we avoid having to resize the heap many times when there are many small mallocs.


Tests
-------------------

To run the tests, run:

make
./tests.exe

Many tests will run.  The first tests allocate 256 integers and check that none of the
values have been corrupted by subsequent my_malloc() calls.

I then free those integers, and re-assign some of them to check that my_free() hasn't
corrupted anything.

I then run some string tests, continuously allocating and copying some strings.
I also check that my_free() calls don't mess up any fof the saved strings.

I then do some invalid parameter tests to make sure that edge cases don't crash the program.

I then do a "silly byte" test where I malloc hundreds of thousands of individual bytes.
I then free half of those bytes and do string testing again.  After freeing the other
half of the silly bytes I check that my strings haven't been corrupted.

Finally, I test that allocating millions of bytes doesn't crash the program or corrupt
the previously allocated strings.

Finally, I test that growing and then shrinking the heap doesn't break anything.