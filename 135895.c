check_location(png_const_structrp png_ptr, int location)
{
   location &= (PNG_HAVE_IHDR|PNG_HAVE_PLTE|PNG_AFTER_IDAT);

   /* New in 1.6.0; copy the location and check it.  This is an API
    * change; previously the app had to use the
    * png_set_unknown_chunk_location API below for each chunk.
    */
   if (location == 0 && (png_ptr->mode & PNG_IS_READ_STRUCT) == 0)
   {
      /* Write struct, so unknown chunks come from the app */
      png_app_warning(png_ptr,
         "png_set_unknown_chunks now expects a valid location");
      /* Use the old behavior */
      location = (png_byte)(png_ptr->mode &
         (PNG_HAVE_IHDR|PNG_HAVE_PLTE|PNG_AFTER_IDAT));
   }

   /* This need not be an internal error - if the app calls
    * png_set_unknown_chunks on a read pointer it must get the location right.
    */
   if (location == 0)
      png_error(png_ptr, "invalid location in png_set_unknown_chunks");

   /* Now reduce the location to the top-most set bit by removing each least
    * significant bit in turn.
    */
   while (location != (location & -location))
      location &= ~(location & -location);

   /* The cast is safe because 'location' is a bit mask and only the low four
    * bits are significant.
    */
   return (png_byte)location;
}