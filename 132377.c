static int __init acpi_parse_madt_ioapic_entries(void)
{
	int count;

	/*
	 * ACPI interpreter is required to complete interrupt setup,
	 * so if it is off, don't enumerate the io-apics with ACPI.
	 * If MPS is present, it will handle them,
	 * otherwise the system will stay in PIC mode
	 */
	if (acpi_disabled || acpi_noirq)
		return -ENODEV;

	if (!boot_cpu_has(X86_FEATURE_APIC))
		return -ENODEV;

	/*
	 * if "noapic" boot option, don't look for IO-APICs
	 */
	if (skip_ioapic_setup) {
		printk(KERN_INFO PREFIX "Skipping IOAPIC probe "
		       "due to 'noapic' option.\n");
		return -ENODEV;
	}

	count = acpi_table_parse_madt(ACPI_MADT_TYPE_IO_APIC, acpi_parse_ioapic,
				      MAX_IO_APICS);
	if (!count) {
		printk(KERN_ERR PREFIX "No IOAPIC entries present\n");
		return -ENODEV;
	} else if (count < 0) {
		printk(KERN_ERR PREFIX "Error parsing IOAPIC entry\n");
		return count;
	}

	count = acpi_table_parse_madt(ACPI_MADT_TYPE_INTERRUPT_OVERRIDE,
				      acpi_parse_int_src_ovr, nr_irqs);
	if (count < 0) {
		printk(KERN_ERR PREFIX
		       "Error parsing interrupt source overrides entry\n");
		/* TBD: Cleanup to allow fallback to MPS */
		return count;
	}

	/*
	 * If BIOS did not supply an INT_SRC_OVR for the SCI
	 * pretend we got one so we can set the SCI flags.
	 */
	if (!acpi_sci_override_gsi)
		acpi_sci_ioapic_setup(acpi_gbl_FADT.sci_interrupt, 0, 0,
				      acpi_gbl_FADT.sci_interrupt);

	/* Fill in identity legacy mappings where no override */
	mp_config_acpi_legacy_irqs();

	count = acpi_table_parse_madt(ACPI_MADT_TYPE_NMI_SOURCE,
				      acpi_parse_nmi_src, nr_irqs);
	if (count < 0) {
		printk(KERN_ERR PREFIX "Error parsing NMI SRC entry\n");
		/* TBD: Cleanup to allow fallback to MPS */
		return count;
	}

	return 0;
}