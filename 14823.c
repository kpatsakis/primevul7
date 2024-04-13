static struct inode *read_inode(unsigned int start_block, unsigned int offset)
{
	static union squashfs_inode_header_1 header;
	long long start = sBlk.s.inode_table_start + start_block;
	long long st = start;
	unsigned int off = offset;
	static struct inode i;
	int res;

	TRACE("read_inode: reading inode [%d:%d]\n", start_block,  offset);

	if(swap) {
		squashfs_base_inode_header_1 sinode;
		res = read_inode_data(&sinode, &st, &off, sizeof(sinode));
		if(res)
			SQUASHFS_SWAP_BASE_INODE_HEADER_1(&header.base, &sinode,
				sizeof(squashfs_base_inode_header_1));
	} else
		res = read_inode_data(&header.base, &st, &off, sizeof(header.base));

	if(res == FALSE)
		EXIT_UNSQUASH("read_inode: failed to read inode %lld:%d\n", st, off);

	i.uid = (uid_t) uid_table[(header.base.inode_type - 1) /
		SQUASHFS_TYPES * 16 + header.base.uid];
	if(header.base.inode_type == SQUASHFS_IPC_TYPE) {
		squashfs_ipc_inode_header_1 *inodep = &header.ipc;

		if(swap) {
			squashfs_ipc_inode_header_1 sinodep;
			res = read_inode_data(&sinodep, &start, &offset, sizeof(sinodep));
			if(res)
				SQUASHFS_SWAP_IPC_INODE_HEADER_1(inodep, &sinodep);
		} else
			res = read_inode_data(inodep, &start, &offset, sizeof(*inodep));

		if(res == FALSE)
			EXIT_UNSQUASH("read_inode: failed to read "
				"inode %lld:%d\n", start, offset);

		if(inodep->type == SQUASHFS_SOCKET_TYPE) {
			i.mode = S_IFSOCK | header.base.mode;
			i.type = SQUASHFS_SOCKET_TYPE;
		} else {
			i.mode = S_IFIFO | header.base.mode;
			i.type = SQUASHFS_FIFO_TYPE;
		}
		i.uid = (uid_t) uid_table[inodep->offset * 16 + inodep->uid];
	} else {
		i.mode = lookup_type[(header.base.inode_type - 1) %
			SQUASHFS_TYPES + 1] | header.base.mode;
		i.type = (header.base.inode_type - 1) % SQUASHFS_TYPES + 1;
	}

	i.xattr = SQUASHFS_INVALID_XATTR;
	i.gid = header.base.guid == 15 ? i.uid :
		(uid_t) guid_table[header.base.guid];
	i.time = sBlk.s.mkfs_time;
	i.inode_number = inode_number ++;

	switch(i.type) {
		case SQUASHFS_DIR_TYPE: {
			squashfs_dir_inode_header_1 *inode = &header.dir;

			if(swap) {
				squashfs_dir_inode_header_1 sinode;
				res = read_inode_data(&sinode, &start, &offset, sizeof(sinode));
				if(res)
					SQUASHFS_SWAP_DIR_INODE_HEADER_1(inode,
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
			squashfs_reg_inode_header_1 *inode = &header.reg;

			if(swap) {
				squashfs_reg_inode_header_1 sinode;
				res = read_inode_data(&sinode, &start, &offset, sizeof(sinode));
				if(res)
					SQUASHFS_SWAP_REG_INODE_HEADER_1(inode,
						&sinode);
			} else
				res = read_inode_data(inode, &start, &offset, sizeof(*inode));

			if(res == FALSE)
				EXIT_UNSQUASH("read_inode: failed to read "
					"inode %lld:%d\n", start, offset);

			i.data = inode->file_size;
			i.time = inode->mtime;
			i.blocks = (i.data + sBlk.s.block_size - 1) >>
				sBlk.s.block_log;
			i.start = inode->start_block;
			i.block_start = start;
			i.block_offset = offset;
			i.fragment = 0;
			i.frag_bytes = 0;
			i.offset = 0;
			i.sparse = 0;
			break;
		}	
		case SQUASHFS_SYMLINK_TYPE: {
			squashfs_symlink_inode_header_1 *inodep =
				&header.symlink;

			if(swap) {
				squashfs_symlink_inode_header_1 sinodep;
				res = read_inode_data(&sinodep, &start, &offset, sizeof(sinodep));
				if(res)
					SQUASHFS_SWAP_SYMLINK_INODE_HEADER_1(inodep,
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
			squashfs_dev_inode_header_1 *inodep = &header.dev;

			if(swap) {
				squashfs_dev_inode_header_1 sinodep;
				res = read_inode_data(&sinodep, &start, &offset, sizeof(sinodep));
				if(res)
					SQUASHFS_SWAP_DEV_INODE_HEADER_1(inodep,
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
		case SQUASHFS_SOCKET_TYPE: {
			i.data = 0;
			break;
			}
		default:
			EXIT_UNSQUASH("Unknown inode type %d in "
				" read_inode_header_1!\n",
				header.base.inode_type);
	}
	return &i;
}