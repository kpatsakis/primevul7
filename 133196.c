static char *display_code(int length, uint32_t code, char *buffer)
{
  int i;

  for (i = 0; i < length; i++)
  {
    int pos = PATTERN_BIT_POS(length, i);
    buffer[i] = ((code >> pos) & 1) == 0 ? '0' : '1';
  }

  buffer[i] = 0;

  return buffer;
}