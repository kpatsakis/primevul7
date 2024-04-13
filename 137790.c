read_image_block(Gif_Reader *grr, uint8_t *buffer, int *bit_pos_store,
		 int *bit_len_store, int bits_needed)
{
  int bit_position = *bit_pos_store;
  int bit_length = *bit_len_store;
  uint8_t block_len;

  while (bit_position + bits_needed > bit_length) {
    /* Read in the next data block. */
    if (bit_position >= 8) {
      /* Need to shift down the upper, unused part of 'buffer' */
      int i = bit_position / 8;
      buffer[0] = buffer[i];
      buffer[1] = buffer[i+1];
      bit_position -= i * 8;
      bit_length -= i * 8;
    }
    block_len = gifgetbyte(grr);
    GIF_DEBUG(("\nimage_block(%d) ", block_len));
    if (block_len == 0) return 0;
    gifgetblock(buffer + bit_length / 8, block_len, grr);
    bit_length += block_len * 8;
  }

  *bit_pos_store = bit_position;
  *bit_len_store = bit_length;
  return 1;
}