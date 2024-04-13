png_set_tRNS(png_structrp png_ptr, png_inforp info_ptr,
    png_const_bytep trans_alpha, int num_trans, png_const_color_16p trans_color)
{
   png_debug1(1, "in %s storage function", "tRNS");

   if (png_ptr == NULL || info_ptr == NULL)

      return;

   if (trans_alpha != NULL)
   {
       /* It may not actually be necessary to set png_ptr->trans_alpha here;
        * we do it for backward compatibility with the way the png_handle_tRNS
        * function used to do the allocation.
        *
        * 1.6.0: The above statement is incorrect; png_handle_tRNS effectively
        * relies on png_set_tRNS storing the information in png_struct
        * (otherwise it won't be there for the code in pngrtran.c).
        */

       png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, 0);

       /* Changed from num_trans to PNG_MAX_PALETTE_LENGTH in version 1.2.1 */
       png_ptr->trans_alpha = info_ptr->trans_alpha = png_voidcast(png_bytep,
         png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH));

       if (num_trans > 0 && num_trans <= PNG_MAX_PALETTE_LENGTH)
          memcpy(info_ptr->trans_alpha, trans_alpha, (png_size_t)num_trans);
   }

   if (trans_color != NULL)
   {
#ifdef PNG_WARNINGS_SUPPORTED
      if (info_ptr->bit_depth < 16)
      {
         int sample_max = (1 << info_ptr->bit_depth) - 1;

         if ((info_ptr->color_type == PNG_COLOR_TYPE_GRAY &&
             trans_color->gray > sample_max) ||
             (info_ptr->color_type == PNG_COLOR_TYPE_RGB &&
             (trans_color->red > sample_max ||
             trans_color->green > sample_max ||
             trans_color->blue > sample_max)))
            png_warning(png_ptr,
               "tRNS chunk has out-of-range samples for bit_depth");
      }
#endif

      info_ptr->trans_color = *trans_color;

      if (num_trans == 0)
         num_trans = 1;
   }

   info_ptr->num_trans = (png_uint_16)num_trans;

   if (num_trans != 0)
   {
      info_ptr->valid |= PNG_INFO_tRNS;
      info_ptr->free_me |= PNG_FREE_TRNS;
   }
}