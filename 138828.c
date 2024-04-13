static int mpage_submit_page(struct mpage_da_data *mpd, struct page *page)
{
	int len;
	loff_t size;
	int err;

	BUG_ON(page->index != mpd->first_page);
	clear_page_dirty_for_io(page);
	/*
	 * We have to be very careful here!  Nothing protects writeback path
	 * against i_size changes and the page can be writeably mapped into
	 * page tables. So an application can be growing i_size and writing
	 * data through mmap while writeback runs. clear_page_dirty_for_io()
	 * write-protects our page in page tables and the page cannot get
	 * written to again until we release page lock. So only after
	 * clear_page_dirty_for_io() we are safe to sample i_size for
	 * ext4_bio_write_page() to zero-out tail of the written page. We rely
	 * on the barrier provided by TestClearPageDirty in
	 * clear_page_dirty_for_io() to make sure i_size is really sampled only
	 * after page tables are updated.
	 */
	size = i_size_read(mpd->inode);
	if (page->index == size >> PAGE_SHIFT)
		len = size & ~PAGE_MASK;
	else
		len = PAGE_SIZE;
	err = ext4_bio_write_page(&mpd->io_submit, page, len, mpd->wbc, false);
	if (!err)
		mpd->wbc->nr_to_write--;
	mpd->first_page++;

	return err;
}