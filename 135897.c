png_set_check_for_invalid_index(png_structrp png_ptr, int allowed)
{
   png_debug(1, "in png_set_check_for_invalid_index");

   if (allowed > 0)
      png_ptr->num_palette_max = 0;

   else
      png_ptr->num_palette_max = -1;
}