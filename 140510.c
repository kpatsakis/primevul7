static void reload_tss(void)
{
	/*
	 * VT restores TR but not its size.  Useless.
	 */
	struct desc_ptr *gdt = this_cpu_ptr(&host_gdt);
	struct desc_struct *descs;

	descs = (void *)gdt->address;
	descs[GDT_ENTRY_TSS].type = 9; /* available TSS */
	load_TR_desc();
}