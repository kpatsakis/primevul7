static void read_block_list(unsigned int *block_list, long long start,
					unsigned int offset, int blocks)
{
	int res;

	TRACE("read_block_list: blocks %d\n", blocks);

	if(swap) {
		unsigned int *block_ptr = malloc(blocks * sizeof(unsigned int));
		if(block_ptr == NULL)
			MEM_ERROR();
		res = read_inode_data(block_ptr, &start, &offset, blocks * sizeof(unsigned int));
		if(res == FALSE)
			EXIT_UNSQUASH("read_block_list: failed to read "
				"inode index %lld:%d\n", start, offset);
		SQUASHFS_SWAP_INTS_3(block_list, block_ptr, blocks);
		free(block_ptr);
	} else {
		res = read_inode_data(block_list, &start, &offset, blocks * sizeof(unsigned int));
		if(res == FALSE)
			EXIT_UNSQUASH("read_block_list: failed to read "
				"inode index %lld:%d\n", start, offset);
	}
}