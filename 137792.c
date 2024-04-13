read_compressed_image(Gif_Image *gfi, Gif_Reader *grr, int read_flags)
{
  if (grr->is_record) {
    const uint32_t image_pos = grr->pos;

    /* scan over image */
    ++grr->pos; /* skip min code size */
    while (grr->pos < grr->length) {
        int amt = grr->v[grr->pos];
        grr->pos += amt + 1;
        if (amt == 0)
            break;
    }
    if (grr->pos > grr->length)
        grr->pos = grr->length;

    gfi->compressed_len = grr->pos - image_pos;
    gfi->compressed_errors = 0;
    if (read_flags & GIF_READ_CONST_RECORD) {
      gfi->compressed = (uint8_t*) &grr->v[image_pos];
      gfi->free_compressed = 0;
    } else {
      gfi->compressed = Gif_NewArray(uint8_t, gfi->compressed_len);
      gfi->free_compressed = Gif_Free;
      if (!gfi->compressed) return 0;
      memcpy(gfi->compressed, &grr->v[image_pos], gfi->compressed_len);
    }

  } else {
    /* non-record; have to read it block by block. */
    uint32_t comp_cap = 1024;
    uint32_t comp_len;
    uint8_t *comp = Gif_NewArray(uint8_t, comp_cap);
    int i;
    if (!comp) return 0;

    /* min code size */
    i = gifgetbyte(grr);
    comp[0] = i;
    comp_len = 1;

    i = gifgetbyte(grr);
    while (i > 0) {
      /* add 2 before check so we don't have to check after loop when appending
	 0 block */
      if (comp_len + i + 2 > comp_cap) {
	comp_cap *= 2;
	Gif_ReArray(comp, uint8_t, comp_cap);
	if (!comp) return 0;
      }
      comp[comp_len] = i;
      gifgetblock(comp + comp_len + 1, i, grr);
      comp_len += i + 1;
      i = gifgetbyte(grr);
    }
    comp[comp_len++] = 0;

    gfi->compressed_len = comp_len;
    gfi->compressed_errors = 0;
    gfi->compressed = comp;
    gfi->free_compressed = Gif_Free;
  }

  return 1;
}