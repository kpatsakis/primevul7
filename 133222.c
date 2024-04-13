static void x3f_setup_camf_text_entry(camf_entry_t *entry)
{
  entry->text_size = *(uint32_t *)entry->value_address;
  entry->text = (char *)entry->value_address + 4;
}