static int mpage_map_and_submit_buffers(struct mpage_da_data *mpd)
{
	struct pagevec pvec;
	int nr_pages, i;
	struct inode *inode = mpd->inode;
	struct buffer_head *head, *bh;
	int bpp_bits = PAGE_SHIFT - inode->i_blkbits;
	pgoff_t start, end;
	ext4_lblk_t lblk;
	sector_t pblock;
	int err;

	start = mpd->map.m_lblk >> bpp_bits;
	end = (mpd->map.m_lblk + mpd->map.m_len - 1) >> bpp_bits;
	lblk = start << bpp_bits;
	pblock = mpd->map.m_pblk;

	pagevec_init(&pvec);
	while (start <= end) {
		nr_pages = pagevec_lookup_range(&pvec, inode->i_mapping,
						&start, end);
		if (nr_pages == 0)
			break;
		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];

			bh = head = page_buffers(page);
			do {
				if (lblk < mpd->map.m_lblk)
					continue;
				if (lblk >= mpd->map.m_lblk + mpd->map.m_len) {
					/*
					 * Buffer after end of mapped extent.
					 * Find next buffer in the page to map.
					 */
					mpd->map.m_len = 0;
					mpd->map.m_flags = 0;
					/*
					 * FIXME: If dioread_nolock supports
					 * blocksize < pagesize, we need to make
					 * sure we add size mapped so far to
					 * io_end->size as the following call
					 * can submit the page for IO.
					 */
					err = mpage_process_page_bufs(mpd, head,
								      bh, lblk);
					pagevec_release(&pvec);
					if (err > 0)
						err = 0;
					return err;
				}
				if (buffer_delay(bh)) {
					clear_buffer_delay(bh);
					bh->b_blocknr = pblock++;
				}
				clear_buffer_unwritten(bh);
			} while (lblk++, (bh = bh->b_this_page) != head);

			/*
			 * FIXME: This is going to break if dioread_nolock
			 * supports blocksize < pagesize as we will try to
			 * convert potentially unmapped parts of inode.
			 */
			mpd->io_submit.io_end->size += PAGE_SIZE;
			/* Page fully mapped - let IO run! */
			err = mpage_submit_page(mpd, page);
			if (err < 0) {
				pagevec_release(&pvec);
				return err;
			}
		}
		pagevec_release(&pvec);
	}
	/* Extent fully mapped and matches with page boundary. We are done. */
	mpd->map.m_len = 0;
	mpd->map.m_flags = 0;
	return 0;
}