#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "constants.h"

int
read_block(int* fd, char* block_original)
{
	int size_read;
	size_read = read(*fd, block_original, BLOCK_SIZE, 0);
	if (size_read == -1) {
		printf("An error ocurred while trying to read a 64KB block.\n");
		printf("ERROR: %s\n", strerror(errno));
		exit(0);
	}
	if (size_read == BLOCK_SIZE)
		printf("Successfully read the whole 64KB block.\n");
	else
		printf("Successfully read %d bytes of data.\n", size_read);
	return size_read;
}

int
determine_block_size(int* compressed_size)
{
	int compressed_block_size;
	if (*compressed_size > BLOCK16)
		compressed_block_size = BLOCK32;
	else
		if (*compressed_size > BLOCK8)
			compressed_block_size = BLOCK16;
		else
			if (*compressed_size > BLOCK4)
				compressed_block_size = BLOCK8;
			else
				if (*compressed_size > BLOCK2)
					compressed_block_size = BLOCK4;
				else
					if (*compressed_size > BLOCK1)
						compressed_block_size = BLOCK2;
					else
						compressed_block_size = BLOCK1;
	return compressed_block_size;
}
