static void copy_vmcs12_to_shadow(struct vcpu_vmx *vmx)
{
	const unsigned long *fields[] = {
		shadow_read_write_fields,
		shadow_read_only_fields
	};
	const int max_fields[] = {
		max_shadow_read_write_fields,
		max_shadow_read_only_fields
	};
	int i, q;
	unsigned long field;
	u64 field_value = 0;
	struct vmcs *shadow_vmcs = vmx->nested.current_shadow_vmcs;

	vmcs_load(shadow_vmcs);

	for (q = 0; q < ARRAY_SIZE(fields); q++) {
		for (i = 0; i < max_fields[q]; i++) {
			field = fields[q][i];
			vmcs12_read_any(&vmx->vcpu, field, &field_value);

			switch (vmcs_field_type(field)) {
			case VMCS_FIELD_TYPE_U16:
				vmcs_write16(field, (u16)field_value);
				break;
			case VMCS_FIELD_TYPE_U32:
				vmcs_write32(field, (u32)field_value);
				break;
			case VMCS_FIELD_TYPE_U64:
				vmcs_write64(field, (u64)field_value);
				break;
			case VMCS_FIELD_TYPE_NATURAL_WIDTH:
				vmcs_writel(field, (long)field_value);
				break;
			}
		}
	}

	vmcs_clear(shadow_vmcs);
	vmcs_load(vmx->loaded_vmcs->vmcs);
}