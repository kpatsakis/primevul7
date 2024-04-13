static uint8_t get_bit(bit_state_t *BS)
{
  if (BS->bit_offset == 8)
  {
    uint8_t byte = *BS->next_address;
    int i;

    for (i = 7; i >= 0; i--)
    {
      BS->bits[i] = byte & 1;
      byte = byte >> 1;
    }
    BS->next_address++;
    BS->bit_offset = 0;
  }

  return BS->bits[BS->bit_offset++];
}