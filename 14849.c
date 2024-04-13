static struct inode *read_inode(unsigned int start_block, unsigned int offset)
{
	static union squashfs_inode_header_2 header;
	long long start = sBlk.s.inode_table_start + start_block;
	long long st = start;
	unsigned int off = offset;
	static struct inode i;
	int res;

	TRACE("read_inode: reading inode [%d:%d]\n", start_block,  offset);

	if(swap) {
		squashfs_base_inode_header_2 sinode;
		res = read_inode_data(&sinode, &st, &off, sizeof(sinode));
		if(res)
			SQUASHFS_SWAP_BASE_INODE_HEADER_2(&header.base, &sinode,
				sizeof(squashfs_base_inode_header_2));
	} else
		res = read_inode_data(&header.base, &st, &off, sizeof(header.base));

	if(res == FALSE)
		EXIT_UNSQUASH("read_inode: failed to read inode %lld:%d\n", st, off);

	i.xattr = SQUASHFS_INVALID_XATTR;
	i.uid = (uid_t) uid_table[header.base.uid];
	i.gid = header.base.guid == SQUASHFS_GUIDS ? i.uid :
		(uid_t) guid_table[header.base.guid];
	i.mode = lookup_type[header.base.inode_type] | header.base.mode;
	i.type = header.base.inode_type;
	i.time = sBlk.s.mkfs_time;
	i.inode_number = inode_number++;

	switch(header.base.inode_type) {
		case SQUASHFS_DIR_TYPE: {
			squashfs_dir_inode_header_2 *inode = &header.dir;

			if(swap) {
				squashfs_dir_inode_header_2 sinode;
				res = read_inode_data(&sinode, &start, &offset, sizeof(sinode));
				if(res)
					SQUASHFS_SWAP_DIR_INODE_HEADER_2(&header.dir,
						&sinode);
			} else
				res = read_inode_data(inode, &start, &offset, sizeof(*inode));

			if(res == FALSE)
				EXIT_UNSQUASH("read_inode: failed to read "
					"inode %lld:%d\n", start, offset);

			i.data = inode->file_size;
			i.offset = inode->offset;
			i.start = inode->start_block;
			i.time = inode->mtime;
			break;
		}
		case SQUASHFS_LDIR_TYPE: {
			squashfs_ldir_inode_header_2 *inode = &header.ldir;

			if(swap) {
				squashfs_ldir_inode_header_2 sinode;
				res = read_inode_data(&sinode, &start, &offset, sizeof(sinode));
				if(res)
					SQUASHFS_SWAP_LDIR_INODE_HEADER_2(&header.ldir,
						&sinode);
			} else
				res = read_inode_data(inode, &start, &offset, sizeof(*inode));

			if(res == FALSE)
				EXIT_UNSQUASH("read_inode: failed to read "
					"inode %lld:%d\n", start, offset);

			i.data = inode->file_size;
			i.offset = inode->offset;
			i.start = inode->start_block;
			i.time = inode->mtime;
			break;
		}
		case SQUASHFS_FILE_TYPE: {
			squashfs_reg_inode_header_2 *inode = &header.reg;

			if(swap) {
				squashfs_reg_inode_header_2 sinode;
				res = read_inode_data(&sinode, &start, &offset, sizeof(sinode));
				if(res)
					SQUASHFS_SWAP_REG_INODE_HEADER_2(inode,
						&sinode);
			} else
				res = read_inode_data(inode, &start, &offset, sizeof(*inode));

			if(res == FALSE)
				EXIT_UNSQUASH("read_inode: failed to read "
					"inode %lld:%d\n", start, offset);

			i.data = inode->file_size;
			i.time = inode->mtime;
			i.frag_bytes = inode->fragment == SQUASHFS_INVALID_FRAG
				?  0 : inode->file_size % sBlk.s.block_size;
			i.fragment = inode->fragment;
			i.offset = inode->offset;
			i.blocks = inode->fragment == SQUASHFS_INVALID_FRAG ?
				(i.data + sBlk.s.block_size - 1) >>
				sBlk.s.block_log : i.data >>
				sBlk.s.block_log;
			i.start = inode->start_block;
			i.block_start = start;
			i.block_offset = offset;
			i.sparse = 0;
			break;
		}	
		case SQUASHFS_SYMLINK_TYPE: {
			squashfs_symlink_inode_header_2 *inodep =
				&header.symlink;

			if(swap) {
				squashfs_symlink_inode_header_2 sinodep;
				res = read_inode_data(&sinodep, &start, &offset, sizeof(sinodep));
				if(res)
					SQUASHFS_SWAP_SYMLINK_INODE_HEADER_2(inodep,
						&sinodep);
			} else
				res = read_inode_data(inodep, &start, &offset, sizeof(*inodep));

			if(res == FALSE)
				EXIT_UNSQUASH("read_inode: failed to read "
					"inode %lld:%d\n", start, offset);

			i.symlink = malloc(inodep->symlink_size + 1);
			if(i.symlink == NULL)
				MEM_ERROR();

			res = read_inode_data(i.symlink, &start, &offset, inodep->symlink_size);
			if(res == FALSE)
				EXIT_UNSQUASH("read_inode: failed to read "
					"inode symbolic link %lld:%d\n", start, offset);
			i.symlink[inodep->symlink_size] = '\0';
			i.data = inodep->symlink_size;
			break;
		}
 		case SQUASHFS_BLKDEV_TYPE:
	 	case SQUASHFS_CHRDEV_TYPE: {
			squashfs_dev_inode_header_2 *inodep = &header.dev;

			if(swap) {
				squashfs_dev_inode_header_2 sinodep;
				res = read_inode_data(&sinodep, &start, &offset, sizeof(sinodep));
				if(res)
					SQUASHFS_SWAP_DEV_INODE_HEADER_2(inodep,
						&sinodep);
			} else
				res = read_inode_data(inodep, &start, &offset, sizeof(*inodep));

			if(res == FALSE)
				EXIT_UNSQUASH("read_inode: failed to read "
					"inode %lld:%d\n", start, offset);

			i.data = inodep->rdev;
			break;
			}
		case SQUASHFS_FIFO_TYPE:
		case SQUASHFS_SOCKET_TYPE:
			i.data = 0;
			break;
		default:
			EXIT_UNSQUASH("Unknown inode type %d in "
				"read_inode_header_2!\n",
				header.base.inode_type);
	}
	return &i;
}