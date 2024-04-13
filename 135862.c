png_set_unknown_chunk_location(png_const_structrp png_ptr, png_inforp info_ptr,
    int chunk, int location)
{
   /* This API is pretty pointless in 1.6.0 because the location can be set
    * before the call to png_set_unknown_chunks.
    *
    * TODO: add a png_app_warning in 1.7
    */
   if (png_ptr != NULL && info_ptr != NULL && chunk >= 0 &&
      chunk < info_ptr->unknown_chunks_num)
   {
      if ((location & (PNG_HAVE_IHDR|PNG_HAVE_PLTE|PNG_AFTER_IDAT)) == 0)
      {
         png_app_error(png_ptr, "invalid unknown chunk location");
         /* Fake out the pre 1.6.0 behavior: */
         if ((location & PNG_HAVE_IDAT) != 0) /* undocumented! */
            location = PNG_AFTER_IDAT;

         else
            location = PNG_HAVE_IHDR; /* also undocumented */
      }

      info_ptr->unknown_chunks[chunk].location =
         check_location(png_ptr, location);
   }
}