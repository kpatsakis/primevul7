png_handle_unknown(png_structrp png_ptr, png_inforp info_ptr,
   png_uint_32 length, int keep)
{
   int handled = 0; /* the chunk was handled */

   png_debug(1, "in png_handle_unknown");

#ifdef PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
   /* NOTE: this code is based on the code in libpng-1.4.12 except for fixing
    * the bug which meant that setting a non-default behavior for a specific
    * chunk would be ignored (the default was always used unless a user
    * callback was installed).
    *
    * 'keep' is the value from the png_chunk_unknown_handling, the setting for
    * this specific chunk_name, if PNG_HANDLE_AS_UNKNOWN_SUPPORTED, if not it
    * will always be PNG_HANDLE_CHUNK_AS_DEFAULT and it needs to be set here.
    * This is just an optimization to avoid multiple calls to the lookup
    * function.
    */
#  ifndef PNG_HANDLE_AS_UNKNOWN_SUPPORTED
#     ifdef PNG_SET_UNKNOWN_CHUNKS_SUPPORTED
   keep = png_chunk_unknown_handling(png_ptr, png_ptr->chunk_name);
#     endif
#  endif

   /* One of the following methods will read the chunk or skip it (at least one
    * of these is always defined because this is the only way to switch on
    * PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
    */
#  ifdef PNG_READ_USER_CHUNKS_SUPPORTED
   /* The user callback takes precedence over the chunk keep value, but the
    * keep value is still required to validate a save of a critical chunk.
    */
   if (png_ptr->read_user_chunk_fn != NULL)
   {
      if (png_cache_unknown_chunk(png_ptr, length) != 0)
      {
         /* Callback to user unknown chunk handler */
         int ret = (*(png_ptr->read_user_chunk_fn))(png_ptr,
            &png_ptr->unknown_chunk);

         /* ret is:
          * negative: An error occurred; png_chunk_error will be called.
          *     zero: The chunk was not handled, the chunk will be discarded
          *           unless png_set_keep_unknown_chunks has been used to set
          *           a 'keep' behavior for this particular chunk, in which
          *           case that will be used.  A critical chunk will cause an
          *           error at this point unless it is to be saved.
          * positive: The chunk was handled, libpng will ignore/discard it.
          */
         if (ret < 0)
            png_chunk_error(png_ptr, "error in user chunk");

         else if (ret == 0)
         {
            /* If the keep value is 'default' or 'never' override it, but
             * still error out on critical chunks unless the keep value is
             * 'always'  While this is weird it is the behavior in 1.4.12.
             * A possible improvement would be to obey the value set for the
             * chunk, but this would be an API change that would probably
             * damage some applications.
             *
             * The png_app_warning below catches the case that matters, where
             * the application has not set specific save or ignore for this
             * chunk or global save or ignore.
             */
            if (keep < PNG_HANDLE_CHUNK_IF_SAFE)
            {
#              ifdef PNG_SET_UNKNOWN_CHUNKS_SUPPORTED
               if (png_ptr->unknown_default < PNG_HANDLE_CHUNK_IF_SAFE)
               {
                  png_chunk_warning(png_ptr, "Saving unknown chunk:");
                  png_app_warning(png_ptr,
                     "forcing save of an unhandled chunk;"
                     " please call png_set_keep_unknown_chunks");
                     /* with keep = PNG_HANDLE_CHUNK_IF_SAFE */
               }
#              endif
               keep = PNG_HANDLE_CHUNK_IF_SAFE;
            }
         }

         else /* chunk was handled */
         {
            handled = 1;
            /* Critical chunks can be safely discarded at this point. */
            keep = PNG_HANDLE_CHUNK_NEVER;
         }
      }

      else
         keep = PNG_HANDLE_CHUNK_NEVER; /* insufficient memory */
   }

   else
   /* Use the SAVE_UNKNOWN_CHUNKS code or skip the chunk */
#  endif /* READ_USER_CHUNKS */

#  ifdef PNG_SAVE_UNKNOWN_CHUNKS_SUPPORTED
   {
      /* keep is currently just the per-chunk setting, if there was no
       * setting change it to the global default now (not that this may
       * still be AS_DEFAULT) then obtain the cache of the chunk if required,
       * if not simply skip the chunk.
       */
      if (keep == PNG_HANDLE_CHUNK_AS_DEFAULT)
         keep = png_ptr->unknown_default;

      if (keep == PNG_HANDLE_CHUNK_ALWAYS ||
         (keep == PNG_HANDLE_CHUNK_IF_SAFE &&
          PNG_CHUNK_ANCILLARY(png_ptr->chunk_name)))
      {
         if (png_cache_unknown_chunk(png_ptr, length) == 0)
            keep = PNG_HANDLE_CHUNK_NEVER;
      }

      else
         png_crc_finish(png_ptr, length);
   }
#  else
#     ifndef PNG_READ_USER_CHUNKS_SUPPORTED
#        error no method to support READ_UNKNOWN_CHUNKS
#     endif

   {
      /* If here there is no read callback pointer set and no support is
       * compiled in to just save the unknown chunks, so simply skip this
       * chunk.  If 'keep' is something other than AS_DEFAULT or NEVER then
       * the app has erroneously asked for unknown chunk saving when there
       * is no support.
       */
      if (keep > PNG_HANDLE_CHUNK_NEVER)
         png_app_error(png_ptr, "no unknown chunk support available");

      png_crc_finish(png_ptr, length);
   }
#  endif

#  ifdef PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
   /* Now store the chunk in the chunk list if appropriate, and if the limits
    * permit it.
    */
   if (keep == PNG_HANDLE_CHUNK_ALWAYS ||
      (keep == PNG_HANDLE_CHUNK_IF_SAFE &&
       PNG_CHUNK_ANCILLARY(png_ptr->chunk_name)))
   {
#     ifdef PNG_USER_LIMITS_SUPPORTED
      switch (png_ptr->user_chunk_cache_max)
      {
         case 2:
            png_ptr->user_chunk_cache_max = 1;
            png_chunk_benign_error(png_ptr, "no space in chunk cache");
            /* FALL THROUGH */
         case 1:
            /* NOTE: prior to 1.6.0 this case resulted in an unknown critical
             * chunk being skipped, now there will be a hard error below.
             */
            break;

         default: /* not at limit */
            --(png_ptr->user_chunk_cache_max);
            /* FALL THROUGH */
         case 0: /* no limit */
#  endif /* USER_LIMITS */
            /* Here when the limit isn't reached or when limits are compiled
             * out; store the chunk.
             */
            png_set_unknown_chunks(png_ptr, info_ptr,
               &png_ptr->unknown_chunk, 1);
            handled = 1;
#  ifdef PNG_USER_LIMITS_SUPPORTED
            break;
      }
#  endif
   }
#  else /* no store support: the chunk must be handled by the user callback */
   PNG_UNUSED(info_ptr)
#  endif

   /* Regardless of the error handling below the cached data (if any) can be
    * freed now.  Notice that the data is not freed if there is a png_error, but
    * it will be freed by destroy_read_struct.
    */
   if (png_ptr->unknown_chunk.data != NULL)
      png_free(png_ptr, png_ptr->unknown_chunk.data);
   png_ptr->unknown_chunk.data = NULL;

#else /* !PNG_READ_UNKNOWN_CHUNKS_SUPPORTED */
   /* There is no support to read an unknown chunk, so just skip it. */
   png_crc_finish(png_ptr, length);
   PNG_UNUSED(info_ptr)
   PNG_UNUSED(keep)
#endif /* !READ_UNKNOWN_CHUNKS */

   /* Check for unhandled critical chunks */
   if (handled == 0 && PNG_CHUNK_CRITICAL(png_ptr->chunk_name))
      png_chunk_error(png_ptr, "unhandled critical chunk");
}