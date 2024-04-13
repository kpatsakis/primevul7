static void read_block_list(unsigned int *block_list, long long start,
					unsigned int offset, int blocks)
{
	int res;

	TRACE("read_block_list: blocks %d\n", blocks);

	res = read_inode_data(block_list, &start, &offset, blocks * sizeof(unsigned int));
	if(res == FALSE)
		EXIT_UNSQUASH("read_block_list: failed to read "
			"inode index %lld:%d\n", start, offset);

	SQUASHFS_INSWAP_INTS(block_list, blocks);
}