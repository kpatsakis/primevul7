static __always_inline u16 vmcs_read16(unsigned long field)
{
	return vmcs_readl(field);
}