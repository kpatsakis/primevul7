png_set_compression_buffer_size(png_structrp png_ptr, png_size_t size)
{
    if (png_ptr == NULL)
       return;

    if (size == 0 || size > PNG_UINT_31_MAX)
       png_error(png_ptr, "invalid compression buffer size");

#  ifdef PNG_SEQUENTIAL_READ_SUPPORTED
      if ((png_ptr->mode & PNG_IS_READ_STRUCT) != 0)
      {
         png_ptr->IDAT_read_size = (png_uint_32)size; /* checked above */
         return;
      }
#  endif

#  ifdef PNG_WRITE_SUPPORTED
      if ((png_ptr->mode & PNG_IS_READ_STRUCT) == 0)
      {
         if (png_ptr->zowner != 0)
         {
            png_warning(png_ptr,
              "Compression buffer size cannot be changed because it is in use");

            return;
         }

#ifndef __COVERITY__
         /* Some compilers complain that this is always false.  However, it
          * can be true when integer overflow happens.
          */
         if (size > ZLIB_IO_MAX)
         {
            png_warning(png_ptr,
               "Compression buffer size limited to system maximum");
            size = ZLIB_IO_MAX; /* must fit */
         }
#endif

         if (size < 6)
         {
            /* Deflate will potentially go into an infinite loop on a SYNC_FLUSH
             * if this is permitted.
             */
            png_warning(png_ptr,
               "Compression buffer size cannot be reduced below 6");

            return;
         }

         if (png_ptr->zbuffer_size != size)
         {
            png_free_buffer_list(png_ptr, &png_ptr->zbuffer_list);
            png_ptr->zbuffer_size = (uInt)size;
         }
      }
#  endif
}