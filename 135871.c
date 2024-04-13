png_read_finish_IDAT(png_structrp png_ptr)
{
   /* We don't need any more data and the stream should have ended, however the
    * LZ end code may actually not have been processed.  In this case we must
    * read it otherwise stray unread IDAT data or, more likely, an IDAT chunk
    * may still remain to be consumed.
    */
   if ((png_ptr->flags & PNG_FLAG_ZSTREAM_ENDED) == 0)
   {
      /* The NULL causes png_read_IDAT_data to swallow any remaining bytes in
       * the compressed stream, but the stream may be damaged too, so even after
       * this call we may need to terminate the zstream ownership.
       */
      png_read_IDAT_data(png_ptr, NULL, 0);
      png_ptr->zstream.next_out = NULL; /* safety */

      /* Now clear everything out for safety; the following may not have been
       * done.
       */
      if ((png_ptr->flags & PNG_FLAG_ZSTREAM_ENDED) == 0)
      {
         png_ptr->mode |= PNG_AFTER_IDAT;
         png_ptr->flags |= PNG_FLAG_ZSTREAM_ENDED;
      }
   }

   /* If the zstream has not been released do it now *and* terminate the reading
    * of the final IDAT chunk.
    */
   if (png_ptr->zowner == png_IDAT)
   {
      /* Always do this; the pointers otherwise point into the read buffer. */
      png_ptr->zstream.next_in = NULL;
      png_ptr->zstream.avail_in = 0;

      /* Now we no longer own the zstream. */
      png_ptr->zowner = 0;

      /* The slightly weird semantics of the sequential IDAT reading is that we
       * are always in or at the end of an IDAT chunk, so we always need to do a
       * crc_finish here.  If idat_size is non-zero we also need to read the
       * spurious bytes at the end of the chunk now.
       */
      (void)png_crc_finish(png_ptr, png_ptr->idat_size);
   }
}