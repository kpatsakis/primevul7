png_handle_sCAL(png_structrp png_ptr, png_inforp info_ptr, png_uint_32 length)
{
   png_bytep buffer;
   png_size_t i;
   int state;

   png_debug(1, "in png_handle_sCAL");

   if ((png_ptr->mode & PNG_HAVE_IHDR) == 0)
      png_chunk_error(png_ptr, "missing IHDR");

   else if ((png_ptr->mode & PNG_HAVE_IDAT) != 0)
   {
      png_crc_finish(png_ptr, length);
      png_chunk_benign_error(png_ptr, "out of place");
      return;
   }

   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_sCAL) != 0)
   {
      png_crc_finish(png_ptr, length);
      png_chunk_benign_error(png_ptr, "duplicate");
      return;
   }

   /* Need unit type, width, \0, height: minimum 4 bytes */
   else if (length < 4)
   {
      png_crc_finish(png_ptr, length);
      png_chunk_benign_error(png_ptr, "invalid");
      return;
   }

   png_debug1(2, "Allocating and reading sCAL chunk data (%u bytes)",
      length + 1);

   buffer = png_read_buffer(png_ptr, length+1, 2/*silent*/);

   if (buffer == NULL)
   {
      png_chunk_benign_error(png_ptr, "out of memory");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_crc_read(png_ptr, buffer, length);
   buffer[length] = 0; /* Null terminate the last string */

   if (png_crc_finish(png_ptr, 0) != 0)
      return;

   /* Validate the unit. */
   if (buffer[0] != 1 && buffer[0] != 2)
   {
      png_chunk_benign_error(png_ptr, "invalid unit");
      return;
   }

   /* Validate the ASCII numbers, need two ASCII numbers separated by
    * a '\0' and they need to fit exactly in the chunk data.
    */
   i = 1;
   state = 0;

   if (png_check_fp_number((png_const_charp)buffer, length, &state, &i) == 0 ||
       i >= length || buffer[i++] != 0)
      png_chunk_benign_error(png_ptr, "bad width format");

   else if (PNG_FP_IS_POSITIVE(state) == 0)
      png_chunk_benign_error(png_ptr, "non-positive width");

   else
   {
      png_size_t heighti = i;

      state = 0;
      if (png_check_fp_number((png_const_charp)buffer, length,
          &state, &i) == 0 || i != length)
         png_chunk_benign_error(png_ptr, "bad height format");

      else if (PNG_FP_IS_POSITIVE(state) == 0)
         png_chunk_benign_error(png_ptr, "non-positive height");

      else
         /* This is the (only) success case. */
         png_set_sCAL_s(png_ptr, info_ptr, buffer[0],
            (png_charp)buffer+1, (png_charp)buffer+heighti);
   }
}