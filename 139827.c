static struct xol_area *__create_xol_area(unsigned long vaddr)
{
	struct mm_struct *mm = current->mm;
	uprobe_opcode_t insn = UPROBE_SWBP_INSN;
	struct xol_area *area;

	area = kmalloc(sizeof(*area), GFP_KERNEL);
	if (unlikely(!area))
		goto out;

	area->bitmap = kzalloc(BITS_TO_LONGS(UINSNS_PER_PAGE) * sizeof(long), GFP_KERNEL);
	if (!area->bitmap)
		goto free_area;

	area->xol_mapping.name = "[uprobes]";
	area->xol_mapping.fault = NULL;
	area->xol_mapping.pages = area->pages;
	area->pages[0] = alloc_page(GFP_HIGHUSER);
	if (!area->pages[0])
		goto free_bitmap;
	area->pages[1] = NULL;

	area->vaddr = vaddr;
	init_waitqueue_head(&area->wq);
	/* Reserve the 1st slot for get_trampoline_vaddr() */
	set_bit(0, area->bitmap);
	atomic_set(&area->slot_count, 1);
	arch_uprobe_copy_ixol(area->pages[0], 0, &insn, UPROBE_SWBP_INSN_SIZE);

	if (!xol_add_vma(mm, area))
		return area;

	__free_page(area->pages[0]);
 free_bitmap:
	kfree(area->bitmap);
 free_area:
	kfree(area);
 out:
	return NULL;
}