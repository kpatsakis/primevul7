static inline bool vmx_control_verify(u32 control, u32 low, u32 high)
{
	/*
	 * Bits 0 in high must be 0, and bits 1 in low must be 1.
	 */
	return ((control & high) | low) == control;
}