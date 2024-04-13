suck_data(char *data, int *store_len, Gif_Reader *grr)
{
  uint8_t len = gifgetbyte(grr);
  int total_len = 0;

  while (len > 0) {
    Gif_ReArray(data, char, total_len + len + 1);
    if (!data) return 0;
    gifgetblock((uint8_t *)data + total_len, len, grr);

    total_len += len;
    data[total_len] = 0;

    len = gifgetbyte(grr);
  }

  if (store_len) *store_len = total_len;
  return data;
}