static int ext4_iomap_begin(struct inode *inode, loff_t offset, loff_t length,
			    unsigned flags, struct iomap *iomap)
{
	struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);
	unsigned int blkbits = inode->i_blkbits;
	unsigned long first_block = offset >> blkbits;
	unsigned long last_block = (offset + length - 1) >> blkbits;
	struct ext4_map_blocks map;
	bool delalloc = false;
	int ret;


	if (flags & IOMAP_REPORT) {
		if (ext4_has_inline_data(inode)) {
			ret = ext4_inline_data_iomap(inode, iomap);
			if (ret != -EAGAIN) {
				if (ret == 0 && offset >= iomap->length)
					ret = -ENOENT;
				return ret;
			}
		}
	} else {
		if (WARN_ON_ONCE(ext4_has_inline_data(inode)))
			return -ERANGE;
	}

	map.m_lblk = first_block;
	map.m_len = last_block - first_block + 1;

	if (flags & IOMAP_REPORT) {
		ret = ext4_map_blocks(NULL, inode, &map, 0);
		if (ret < 0)
			return ret;

		if (ret == 0) {
			ext4_lblk_t end = map.m_lblk + map.m_len - 1;
			struct extent_status es;

			ext4_es_find_delayed_extent_range(inode, map.m_lblk, end, &es);

			if (!es.es_len || es.es_lblk > end) {
				/* entire range is a hole */
			} else if (es.es_lblk > map.m_lblk) {
				/* range starts with a hole */
				map.m_len = es.es_lblk - map.m_lblk;
			} else {
				ext4_lblk_t offs = 0;

				if (es.es_lblk < map.m_lblk)
					offs = map.m_lblk - es.es_lblk;
				map.m_lblk = es.es_lblk + offs;
				map.m_len = es.es_len - offs;
				delalloc = true;
			}
		}
	} else if (flags & IOMAP_WRITE) {
		int dio_credits;
		handle_t *handle;
		int retries = 0;

		/* Trim mapping request to maximum we can map at once for DIO */
		if (map.m_len > DIO_MAX_BLOCKS)
			map.m_len = DIO_MAX_BLOCKS;
		dio_credits = ext4_chunk_trans_blocks(inode, map.m_len);
retry:
		/*
		 * Either we allocate blocks and then we don't get unwritten
		 * extent so we have reserved enough credits, or the blocks
		 * are already allocated and unwritten and in that case
		 * extent conversion fits in the credits as well.
		 */
		handle = ext4_journal_start(inode, EXT4_HT_MAP_BLOCKS,
					    dio_credits);
		if (IS_ERR(handle))
			return PTR_ERR(handle);

		ret = ext4_map_blocks(handle, inode, &map,
				      EXT4_GET_BLOCKS_CREATE_ZERO);
		if (ret < 0) {
			ext4_journal_stop(handle);
			if (ret == -ENOSPC &&
			    ext4_should_retry_alloc(inode->i_sb, &retries))
				goto retry;
			return ret;
		}

		/*
		 * If we added blocks beyond i_size, we need to make sure they
		 * will get truncated if we crash before updating i_size in
		 * ext4_iomap_end(). For faults we don't need to do that (and
		 * even cannot because for orphan list operations inode_lock is
		 * required) - if we happen to instantiate block beyond i_size,
		 * it is because we race with truncate which has already added
		 * the inode to the orphan list.
		 */
		if (!(flags & IOMAP_FAULT) && first_block + map.m_len >
		    (i_size_read(inode) + (1 << blkbits) - 1) >> blkbits) {
			int err;

			err = ext4_orphan_add(handle, inode);
			if (err < 0) {
				ext4_journal_stop(handle);
				return err;
			}
		}
		ext4_journal_stop(handle);
	} else {
		ret = ext4_map_blocks(NULL, inode, &map, 0);
		if (ret < 0)
			return ret;
	}

	iomap->flags = 0;
	if (ext4_inode_datasync_dirty(inode))
		iomap->flags |= IOMAP_F_DIRTY;
	iomap->bdev = inode->i_sb->s_bdev;
	iomap->dax_dev = sbi->s_daxdev;
	iomap->offset = (u64)first_block << blkbits;
	iomap->length = (u64)map.m_len << blkbits;

	if (ret == 0) {
		iomap->type = delalloc ? IOMAP_DELALLOC : IOMAP_HOLE;
		iomap->addr = IOMAP_NULL_ADDR;
	} else {
		if (map.m_flags & EXT4_MAP_MAPPED) {
			iomap->type = IOMAP_MAPPED;
		} else if (map.m_flags & EXT4_MAP_UNWRITTEN) {
			iomap->type = IOMAP_UNWRITTEN;
		} else {
			WARN_ON_ONCE(1);
			return -EIO;
		}
		iomap->addr = (u64)map.m_pblk << blkbits;
	}

	if (map.m_flags & EXT4_MAP_NEW)
		iomap->flags |= IOMAP_F_NEW;

	return 0;
}