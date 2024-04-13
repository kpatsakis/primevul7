static inline bool vmcs12_read_any(struct kvm_vcpu *vcpu,
					unsigned long field, u64 *ret)
{
	short offset = vmcs_field_to_offset(field);
	char *p;

	if (offset < 0)
		return 0;

	p = ((char *)(get_vmcs12(vcpu))) + offset;

	switch (vmcs_field_type(field)) {
	case VMCS_FIELD_TYPE_NATURAL_WIDTH:
		*ret = *((natural_width *)p);
		return 1;
	case VMCS_FIELD_TYPE_U16:
		*ret = *((u16 *)p);
		return 1;
	case VMCS_FIELD_TYPE_U32:
		*ret = *((u32 *)p);
		return 1;
	case VMCS_FIELD_TYPE_U64:
		*ret = *((u64 *)p);
		return 1;
	default:
		return 0; /* can never happen. */
	}
}