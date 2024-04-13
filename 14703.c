pxa3xx_gcu_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned int size = vma->vm_end - vma->vm_start;
	struct pxa3xx_gcu_priv *priv = to_pxa3xx_gcu_priv(file);

	switch (vma->vm_pgoff) {
	case 0:
		/* hand out the shared data area */
		if (size != SHARED_SIZE)
			return -EINVAL;

		return dma_mmap_coherent(priv->dev, vma,
			priv->shared, priv->shared_phys, size);

	case SHARED_SIZE >> PAGE_SHIFT:
		/* hand out the MMIO base for direct register access
		 * from userspace */
		if (size != resource_size(priv->resource_mem))
			return -EINVAL;

		vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

		return io_remap_pfn_range(vma, vma->vm_start,
				priv->resource_mem->start >> PAGE_SHIFT,
				size, vma->vm_page_prot);
	}

	return -EINVAL;
}