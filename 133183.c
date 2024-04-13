static void x3f_setup_camf_property_entry(camf_entry_t *entry)
{
  int i;
  uint8_t *e = (uint8_t *)entry->entry;
  uint8_t *v = (uint8_t *)entry->value_address;
  uint32_t num = entry->property_num = *(uint32_t *)v;
  uint32_t off = *(uint32_t *)(v + 4);

  entry->property_name = (char **)malloc(num * sizeof(uint8_t *));
  entry->property_value = (uint8_t **)malloc(num * sizeof(uint8_t *));

  for (i = 0; i < num; i++)
  {
    uint32_t name_off = off + *(uint32_t *)(v + 8 + 8 * i);
    uint32_t value_off = off + *(uint32_t *)(v + 8 + 8 * i + 4);

    entry->property_name[i] = (char *)(e + name_off);
    entry->property_value[i] = e + value_off;
  }
}