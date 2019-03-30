#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

int buffer;
void init_buffer() { buffer = 0; }
int buffer_empty() { return (buffer==0); }
int buffer_full() { return (buffer==1); }
void produce_to_buffer()
{
        if (buffer==1) { printf("ERROR: Buffer overflow\n"); exit(1); }
	else buffer++;
}

void consume_from_buffer()
{
        if (buffer==0) { printf("ERROR: Buffer underflow\n"); exit(1); }
	else buffer--;
}
