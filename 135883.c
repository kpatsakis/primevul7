png_set_unknown_chunks(png_const_structrp png_ptr,
   png_inforp info_ptr, png_const_unknown_chunkp unknowns, int num_unknowns)
{
   png_unknown_chunkp np;

   if (png_ptr == NULL || info_ptr == NULL || num_unknowns <= 0 ||
       unknowns == NULL)
      return;

   /* Check for the failure cases where support has been disabled at compile
    * time.  This code is hardly ever compiled - it's here because
    * STORE_UNKNOWN_CHUNKS is set by both read and write code (compiling in this
    * code) but may be meaningless if the read or write handling of unknown
    * chunks is not compiled in.
    */
#  if !defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED) && \
      defined(PNG_READ_SUPPORTED)
      if ((png_ptr->mode & PNG_IS_READ_STRUCT) != 0)
      {
         png_app_error(png_ptr, "no unknown chunk support on read");

         return;
      }
#  endif
#  if !defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED) && \
      defined(PNG_WRITE_SUPPORTED)
      if ((png_ptr->mode & PNG_IS_READ_STRUCT) == 0)
      {
         png_app_error(png_ptr, "no unknown chunk support on write");

         return;
      }
#  endif

   /* Prior to 1.6.0 this code used png_malloc_warn; however, this meant that
    * unknown critical chunks could be lost with just a warning resulting in
    * undefined behavior.  Now png_chunk_report is used to provide behavior
    * appropriate to read or write.
    */
   np = png_voidcast(png_unknown_chunkp, png_realloc_array(png_ptr,
         info_ptr->unknown_chunks, info_ptr->unknown_chunks_num, num_unknowns,
         sizeof *np));

   if (np == NULL)
   {
      png_chunk_report(png_ptr, "too many unknown chunks",
         PNG_CHUNK_WRITE_ERROR);

      return;
   }

   png_free(png_ptr, info_ptr->unknown_chunks);
   info_ptr->unknown_chunks = np; /* safe because it is initialized */
   info_ptr->free_me |= PNG_FREE_UNKN;

   np += info_ptr->unknown_chunks_num;

   /* Increment unknown_chunks_num each time round the loop to protect the
    * just-allocated chunk data.
    */
   for (; num_unknowns > 0; --num_unknowns, ++unknowns)
   {
      memcpy(np->name, unknowns->name, (sizeof np->name));
      np->name[(sizeof np->name)-1] = '\0';
      np->location = check_location(png_ptr, unknowns->location);

      if (unknowns->size == 0)
      {
         np->data = NULL;
         np->size = 0;
      }

      else
      {
         np->data = png_voidcast(png_bytep,
            png_malloc_base(png_ptr, unknowns->size));

         if (np->data == NULL)
         {
            png_chunk_report(png_ptr, "unknown chunk: out of memory",
               PNG_CHUNK_WRITE_ERROR);
            /* But just skip storing the unknown chunk */
            continue;
         }

         memcpy(np->data, unknowns->data, unknowns->size);
         np->size = unknowns->size;
      }

      /* These increments are skipped on out-of-memory for the data - the
       * unknown chunk entry gets overwritten if the png_chunk_report returns.
       * This is correct in the read case (the chunk is just dropped.)
       */
      ++np;
      ++(info_ptr->unknown_chunks_num);
   }
}