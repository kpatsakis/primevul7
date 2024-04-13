static void read_block_list(unsigned int *block_list, long long start,
	unsigned int offset, int blocks)
{
	unsigned short *source;
	int i, res;

	TRACE("read_block_list: blocks %d\n", blocks);

	source = malloc(blocks * sizeof(unsigned short));
	if(source == NULL)
		MEM_ERROR();

	if(swap) {
		char *swap_buff;

		swap_buff = malloc(blocks * sizeof(unsigned short));
		if(swap_buff == NULL)
			MEM_ERROR();

		res = read_inode_data(swap_buff, &start, &offset, blocks * sizeof(unsigned short));
		if(res == FALSE)
			EXIT_UNSQUASH("read_block_list: failed to read "
					"inode index %lld:%d\n", start, offset);
		SQUASHFS_SWAP_SHORTS_3(source, swap_buff, blocks);
		free(swap_buff);
	} else {
		res = read_inode_data(source, &start, &offset, blocks * sizeof(unsigned short));
		if(res == FALSE)
			EXIT_UNSQUASH("read_block_list: failed to read "
					"inode index %lld:%d\n", start, offset);
	}

	for(i = 0; i < blocks; i++)
		block_list[i] = SQUASHFS_COMPRESSED_SIZE(source[i]) |
			(SQUASHFS_COMPRESSED(source[i]) ? 0 :
			SQUASHFS_COMPRESSED_BIT_BLOCK);
	free(source);
}