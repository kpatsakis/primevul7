read_image_data(Gif_Context *gfc, Gif_Reader *grr)
{
  /* we need a bit more than GIF_MAX_BLOCK in case a single code is split
     across blocks */
  uint8_t buffer[GIF_MAX_BLOCK + 5];
  int i;
  uint32_t accum;

  int bit_position;
  int bit_length;

  Gif_Code code;
  Gif_Code old_code;
  Gif_Code clear_code;
  Gif_Code eoi_code;
  Gif_Code next_code;
#define CUR_BUMP_CODE (1 << bits_needed)
#define CUR_CODE_MASK ((1 << bits_needed) - 1)

  int min_code_size;
  int bits_needed;

  gfc->decodepos = 0;

  min_code_size = gifgetbyte(grr);
  GIF_DEBUG(("\n\nmin_code_size(%d) ", min_code_size));
  if (min_code_size >= GIF_MAX_CODE_BITS) {
    gif_read_error(gfc, 1, "image corrupted, min_code_size too big");
    min_code_size = GIF_MAX_CODE_BITS - 1;
  } else if (min_code_size < 2) {
    gif_read_error(gfc, 1, "image corrupted, min_code_size too small");
    min_code_size = 2;
  }
  clear_code = 1 << min_code_size;
  for (code = 0; code < clear_code; code++) {
    gfc->prefix[code] = 49428;
    gfc->suffix[code] = (uint8_t)code;
    gfc->length[code] = 1;
  }
  eoi_code = clear_code + 1;

  next_code = eoi_code;
  bits_needed = min_code_size + 1;

  code = clear_code;

  bit_length = bit_position = 0;
  /* Thus the 'Read in the next data block.' code below will be invoked on the
     first time through: exactly right! */

  while (1) {

    old_code = code;

    /* GET A CODE INTO THE 'code' VARIABLE.
     *
     * 9.Dec.1998 - Rather than maintain a byte pointer and a bit offset into
     * the current byte (and the processing associated with that), we maintain
     * one number: the offset, in bits, from the beginning of 'buffer'. This
     * much cleaner choice was inspired by Patrick J. Naughton
     * <naughton@wind.sun.com>'s GIF-reading code, which does the same thing.
     * His code distributed as part of XV in xvgif.c. */

    if (bit_position + bits_needed > bit_length)
      /* Read in the next data block. */
      if (!read_image_block(grr, buffer, &bit_position, &bit_length,
			    bits_needed))
	goto zero_length_block;

    i = bit_position / 8;
    accum = buffer[i] + (buffer[i+1] << 8);
    if (bits_needed >= 8)
      accum |= (buffer[i+2]) << 16;
    code = (Gif_Code)((accum >> (bit_position % 8)) & CUR_CODE_MASK);
    bit_position += bits_needed;

    GIF_DEBUG(("%d ", code));

    /* CHECK FOR SPECIAL OR BAD CODES: clear_code, eoi_code, or a code that is
     * too large. */
    if (code == clear_code) {
      GIF_DEBUG(("clear "));
      bits_needed = min_code_size + 1;
      next_code = eoi_code;
      continue;

    } else if (code == eoi_code)
      break;

    else if (code > next_code && next_code && next_code != clear_code) {
      /* code > next_code: a (hopefully recoverable) error.

	 Bug fix, 5/27: Do this even if old_code == clear_code, and set code
	 to 0 to prevent errors later. (If we didn't zero code, we'd later set
	 old_code = code; then we had old_code >= next_code; so the prefixes
	 array got all screwed up!)

	 Bug fix, 4/12/2010: It is not an error if next_code == clear_code.
	 This happens at the end of a large GIF: see the next comment ("If no
	 meaningful next code should be defined...."). */
      if (gfc->errors[1] < 20)
          gif_read_error(gfc, 1, "image corrupted, code out of range");
      else if (gfc->errors[1] == 20)
          gif_read_error(gfc, 1, "(not reporting more errors)");
      code = 0;
    }

    /* PROCESS THE CURRENT CODE and define the next code. If no meaningful
     * next code should be defined, then we have set next_code to either
     * 'eoi_code' or 'clear_code' -- so we'll store useless prefix/suffix data
     * in a useless place. */

    /* *First,* set up the prefix and length for the next code
       (in case code == next_code). */
    gfc->prefix[next_code] = old_code;
    gfc->length[next_code] = gfc->length[old_code] + 1;

    /* Use one_code to process code. It's nice that it returns the first
       pixel in code: that's what we need. */
    gfc->suffix[next_code] = one_code(gfc, code);

    /* Special processing if code == next_code: we didn't know code's final
       suffix when we called one_code, but we do now. */
    /* 7.Mar.2014 -- Avoid error if image has zero width/height. */
    if (code == next_code && gfc->image + gfc->decodepos <= gfc->maximage)
      gfc->image[gfc->decodepos - 1] = gfc->suffix[next_code];

    /* Increment next_code except for the 'clear_code' special case (that's
       when we're reading at the end of a GIF) */
    if (next_code != clear_code) {
      next_code++;
      if (next_code == CUR_BUMP_CODE) {
	if (bits_needed < GIF_MAX_CODE_BITS)
	  bits_needed++;
	else
	  next_code = clear_code;
      }
    }

  }

  /* read blocks until zero-length reached. */
  i = gifgetbyte(grr);
  GIF_DEBUG(("\nafter_image(%d)\n", i));
  while (i > 0) {
    gifgetblock(buffer, i, grr);
    i = gifgetbyte(grr);
    GIF_DEBUG(("\nafter_image(%d)\n", i));
  }

  /* zero-length block reached. */
 zero_length_block: {
      long delta = (long) (gfc->maximage - gfc->image) - (long) gfc->decodepos;
      char buf[BUFSIZ];
      if (delta > 0) {
          sprintf(buf, "missing %ld %s of image data", delta,
                  delta == 1 ? "pixel" : "pixels");
          gif_read_error(gfc, 1, buf);
          memset(&gfc->image[gfc->decodepos], 0, delta);
      } else if (delta < -1) {
          /* One pixel of superfluous data is OK; that could be the
             code == next_code case. */
          sprintf(buf, "%ld superfluous pixels of image data", -delta);
          gif_read_error(gfc, 0, buf);
      }
  }
}