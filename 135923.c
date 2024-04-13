png_set_sPLT(png_const_structrp png_ptr,
    png_inforp info_ptr, png_const_sPLT_tp entries, int nentries)
/*
 *  entries        - array of png_sPLT_t structures
 *                   to be added to the list of palettes
 *                   in the info structure.
 *
 *  nentries       - number of palette structures to be
 *                   added.
 */
{
   png_sPLT_tp np;

   if (png_ptr == NULL || info_ptr == NULL || nentries <= 0 || entries == NULL)
      return;

   /* Use the internal realloc function, which checks for all the possible
    * overflows.  Notice that the parameters are (int) and (size_t)
    */
   np = png_voidcast(png_sPLT_tp,png_realloc_array(png_ptr,
      info_ptr->splt_palettes, info_ptr->splt_palettes_num, nentries,
      sizeof *np));

   if (np == NULL)
   {
      /* Out of memory or too many chunks */
      png_chunk_report(png_ptr, "too many sPLT chunks", PNG_CHUNK_WRITE_ERROR);

      return;
   }

   png_free(png_ptr, info_ptr->splt_palettes);
   info_ptr->splt_palettes = np;
   info_ptr->free_me |= PNG_FREE_SPLT;

   np += info_ptr->splt_palettes_num;

   do
   {
      png_size_t length;

      /* Skip invalid input entries */
      if (entries->name == NULL || entries->entries == NULL)
      {
         /* png_handle_sPLT doesn't do this, so this is an app error */
         png_app_error(png_ptr, "png_set_sPLT: invalid sPLT");
         /* Just skip the invalid entry */
         continue;
      }

      np->depth = entries->depth;

      /* In the event of out-of-memory just return - there's no point keeping
       * on trying to add sPLT chunks.
       */
      length = strlen(entries->name) + 1;
      np->name = png_voidcast(png_charp, png_malloc_base(png_ptr, length));

      if (np->name == NULL)
         break;

      memcpy(np->name, entries->name, length);

      /* IMPORTANT: we have memory now that won't get freed if something else
       * goes wrong; this code must free it.  png_malloc_array produces no
       * warnings; use a png_chunk_report (below) if there is an error.
       */
      np->entries = png_voidcast(png_sPLT_entryp, png_malloc_array(png_ptr,
          entries->nentries, sizeof (png_sPLT_entry)));

      if (np->entries == NULL)
      {
         png_free(png_ptr, np->name);
         np->name = NULL;
         break;
      }

      np->nentries = entries->nentries;
      /* This multiply can't overflow because png_malloc_array has already
       * checked it when doing the allocation.
       */
      memcpy(np->entries, entries->entries,
         entries->nentries * sizeof (png_sPLT_entry));

      /* Note that 'continue' skips the advance of the out pointer and out
       * count, so an invalid entry is not added.
       */
      info_ptr->valid |= PNG_INFO_sPLT;
      ++(info_ptr->splt_palettes_num);
      ++np;
   }
   while (++entries, --nentries);

   if (nentries > 0)
      png_chunk_report(png_ptr, "sPLT out of memory", PNG_CHUNK_WRITE_ERROR);
}