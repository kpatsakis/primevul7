png_handle_IEND(png_structrp png_ptr, png_inforp info_ptr, png_uint_32 length)
{
   png_debug(1, "in png_handle_IEND");

   if ((png_ptr->mode & PNG_HAVE_IHDR) == 0 ||
       (png_ptr->mode & PNG_HAVE_IDAT) == 0)
      png_chunk_error(png_ptr, "out of place");

   png_ptr->mode |= (PNG_AFTER_IDAT | PNG_HAVE_IEND);

   png_crc_finish(png_ptr, length);

   if (length != 0)
      png_chunk_benign_error(png_ptr, "invalid");

   PNG_UNUSED(info_ptr)
}