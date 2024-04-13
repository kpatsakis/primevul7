static void init_vmcs_shadow_fields(void)
{
	int i, j;

	/* No checks for read only fields yet */

	for (i = j = 0; i < max_shadow_read_write_fields; i++) {
		switch (shadow_read_write_fields[i]) {
		case GUEST_BNDCFGS:
			if (!vmx_mpx_supported())
				continue;
			break;
		default:
			break;
		}

		if (j < i)
			shadow_read_write_fields[j] =
				shadow_read_write_fields[i];
		j++;
	}
	max_shadow_read_write_fields = j;

	/* shadowed fields guest access without vmexit */
	for (i = 0; i < max_shadow_read_write_fields; i++) {
		clear_bit(shadow_read_write_fields[i],
			  vmx_vmwrite_bitmap);
		clear_bit(shadow_read_write_fields[i],
			  vmx_vmread_bitmap);
	}
	for (i = 0; i < max_shadow_read_only_fields; i++)
		clear_bit(shadow_read_only_fields[i],
			  vmx_vmread_bitmap);
}