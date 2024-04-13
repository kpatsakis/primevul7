static void mpage_release_unused_pages(struct mpage_da_data *mpd,
				       bool invalidate)
{
	int nr_pages, i;
	pgoff_t index, end;
	struct pagevec pvec;
	struct inode *inode = mpd->inode;
	struct address_space *mapping = inode->i_mapping;

	/* This is necessary when next_page == 0. */
	if (mpd->first_page >= mpd->next_page)
		return;

	index = mpd->first_page;
	end   = mpd->next_page - 1;
	if (invalidate) {
		ext4_lblk_t start, last;
		start = index << (PAGE_SHIFT - inode->i_blkbits);
		last = end << (PAGE_SHIFT - inode->i_blkbits);
		ext4_es_remove_extent(inode, start, last - start + 1);
	}

	pagevec_init(&pvec);
	while (index <= end) {
		nr_pages = pagevec_lookup_range(&pvec, mapping, &index, end);
		if (nr_pages == 0)
			break;
		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];

			BUG_ON(!PageLocked(page));
			BUG_ON(PageWriteback(page));
			if (invalidate) {
				if (page_mapped(page))
					clear_page_dirty_for_io(page);
				block_invalidatepage(page, 0, PAGE_SIZE);
				ClearPageUptodate(page);
			}
			unlock_page(page);
		}
		pagevec_release(&pvec);
	}
}