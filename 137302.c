BPF_CALL_4(bpf_msg_pull_data,
	   struct sk_msg_buff *, msg, u32, start, u32, end, u64, flags)
{
	unsigned int len = 0, offset = 0, copy = 0;
	struct scatterlist *sg = msg->sg_data;
	int first_sg, last_sg, i, shift;
	unsigned char *p, *to, *from;
	int bytes = end - start;
	struct page *page;

	if (unlikely(flags || end <= start))
		return -EINVAL;

	/* First find the starting scatterlist element */
	i = msg->sg_start;
	do {
		len = sg[i].length;
		offset += len;
		if (start < offset + len)
			break;
		i++;
		if (i == MAX_SKB_FRAGS)
			i = 0;
	} while (i != msg->sg_end);

	if (unlikely(start >= offset + len))
		return -EINVAL;

	if (!msg->sg_copy[i] && bytes <= len)
		goto out;

	first_sg = i;

	/* At this point we need to linearize multiple scatterlist
	 * elements or a single shared page. Either way we need to
	 * copy into a linear buffer exclusively owned by BPF. Then
	 * place the buffer in the scatterlist and fixup the original
	 * entries by removing the entries now in the linear buffer
	 * and shifting the remaining entries. For now we do not try
	 * to copy partial entries to avoid complexity of running out
	 * of sg_entry slots. The downside is reading a single byte
	 * will copy the entire sg entry.
	 */
	do {
		copy += sg[i].length;
		i++;
		if (i == MAX_SKB_FRAGS)
			i = 0;
		if (bytes < copy)
			break;
	} while (i != msg->sg_end);
	last_sg = i;

	if (unlikely(copy < end - start))
		return -EINVAL;

	page = alloc_pages(__GFP_NOWARN | GFP_ATOMIC, get_order(copy));
	if (unlikely(!page))
		return -ENOMEM;
	p = page_address(page);
	offset = 0;

	i = first_sg;
	do {
		from = sg_virt(&sg[i]);
		len = sg[i].length;
		to = p + offset;

		memcpy(to, from, len);
		offset += len;
		sg[i].length = 0;
		put_page(sg_page(&sg[i]));

		i++;
		if (i == MAX_SKB_FRAGS)
			i = 0;
	} while (i != last_sg);

	sg[first_sg].length = copy;
	sg_set_page(&sg[first_sg], page, copy, 0);

	/* To repair sg ring we need to shift entries. If we only
	 * had a single entry though we can just replace it and
	 * be done. Otherwise walk the ring and shift the entries.
	 */
	shift = last_sg - first_sg - 1;
	if (!shift)
		goto out;

	i = first_sg + 1;
	do {
		int move_from;

		if (i + shift >= MAX_SKB_FRAGS)
			move_from = i + shift - MAX_SKB_FRAGS;
		else
			move_from = i + shift;

		if (move_from == msg->sg_end)
			break;

		sg[i] = sg[move_from];
		sg[move_from].length = 0;
		sg[move_from].page_link = 0;
		sg[move_from].offset = 0;

		i++;
		if (i == MAX_SKB_FRAGS)
			i = 0;
	} while (1);
	msg->sg_end -= shift;
	if (msg->sg_end < 0)
		msg->sg_end += MAX_SKB_FRAGS;
out:
	msg->data = sg_virt(&sg[i]) + start - offset;
	msg->data_end = msg->data + bytes;

	return 0;
}