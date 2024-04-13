acpi_parse_ioapic(struct acpi_subtable_header * header, const unsigned long end)
{
	struct acpi_madt_io_apic *ioapic = NULL;
	struct ioapic_domain_cfg cfg = {
		.type = IOAPIC_DOMAIN_DYNAMIC,
		.ops = &mp_ioapic_irqdomain_ops,
	};

	ioapic = (struct acpi_madt_io_apic *)header;

	if (BAD_MADT_ENTRY(ioapic, end))
		return -EINVAL;

	acpi_table_print_madt_entry(header);

	/* Statically assign IRQ numbers for IOAPICs hosting legacy IRQs */
	if (ioapic->global_irq_base < nr_legacy_irqs())
		cfg.type = IOAPIC_DOMAIN_LEGACY;

	mp_register_ioapic(ioapic->id, ioapic->address, ioapic->global_irq_base,
			   &cfg);

	return 0;
}