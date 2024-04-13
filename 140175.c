static inline u64 vmx_control_msr(u32 low, u32 high)
{
	return low | ((u64)high << 32);
}