acpi_parse_x2apic(struct acpi_subtable_header *header, const unsigned long end)
{
	struct acpi_madt_local_x2apic *processor = NULL;
	int apic_id;
	u8 enabled;

	processor = (struct acpi_madt_local_x2apic *)header;

	if (BAD_MADT_ENTRY(processor, end))
		return -EINVAL;

	acpi_table_print_madt_entry(header);

	apic_id = processor->local_apic_id;
	enabled = processor->lapic_flags & ACPI_MADT_ENABLED;
#ifdef CONFIG_X86_X2APIC
	/*
	 * We need to register disabled CPU as well to permit
	 * counting disabled CPUs. This allows us to size
	 * cpus_possible_map more accurately, to permit
	 * to not preallocating memory for all NR_CPUS
	 * when we use CPU hotplug.
	 */
	if (!apic->apic_id_valid(apic_id) && enabled)
		printk(KERN_WARNING PREFIX "x2apic entry ignored\n");
	else
		acpi_register_lapic(apic_id, processor->uid, enabled);
#else
	printk(KERN_WARNING PREFIX "x2apic entry ignored\n");
#endif

	return 0;
}