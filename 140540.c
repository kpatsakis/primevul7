static inline int vmcs_field_type(unsigned long field)
{
	if (0x1 & field)	/* the *_HIGH fields are all 32 bit */
		return VMCS_FIELD_TYPE_U32;
	return (field >> 13) & 0x3 ;
}