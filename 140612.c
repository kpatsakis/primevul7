static __always_inline u32 vmcs_read32(unsigned long field)
{
	return vmcs_readl(field);
}