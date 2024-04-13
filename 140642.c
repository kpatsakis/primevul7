static inline short vmcs_field_to_offset(unsigned long field)
{
	if (field >= max_vmcs_field || vmcs_field_to_offset_table[field] == 0)
		return -1;
	return vmcs_field_to_offset_table[field];
}