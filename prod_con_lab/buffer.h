#ifndef BUFFER_H
#define BUFFER_H 1
void init_buffer();
int buffer_empty();
int buffer_full();
void produce_to_buffer();
void consume_from_buffer();
#endif
