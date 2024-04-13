acpi_parse_int_src_ovr(struct acpi_subtable_header * header,
		       const unsigned long end)
{
	struct acpi_madt_interrupt_override *intsrc = NULL;

	intsrc = (struct acpi_madt_interrupt_override *)header;

	if (BAD_MADT_ENTRY(intsrc, end))
		return -EINVAL;

	acpi_table_print_madt_entry(header);

	if (intsrc->source_irq == acpi_gbl_FADT.sci_interrupt) {
		acpi_sci_ioapic_setup(intsrc->source_irq,
				      intsrc->inti_flags & ACPI_MADT_POLARITY_MASK,
				      (intsrc->inti_flags & ACPI_MADT_TRIGGER_MASK) >> 2,
				      intsrc->global_irq);
		return 0;
	}

	if (intsrc->source_irq == 0) {
		if (acpi_skip_timer_override) {
			printk(PREFIX "BIOS IRQ0 override ignored.\n");
			return 0;
		}

		if ((intsrc->global_irq == 2) && acpi_fix_pin2_polarity
			&& (intsrc->inti_flags & ACPI_MADT_POLARITY_MASK)) {
			intsrc->inti_flags &= ~ACPI_MADT_POLARITY_MASK;
			printk(PREFIX "BIOS IRQ0 pin2 override: forcing polarity to high active.\n");
		}
	}

	mp_override_legacy_irq(intsrc->source_irq,
				intsrc->inti_flags & ACPI_MADT_POLARITY_MASK,
				(intsrc->inti_flags & ACPI_MADT_TRIGGER_MASK) >> 2,
				intsrc->global_irq);

	return 0;
}