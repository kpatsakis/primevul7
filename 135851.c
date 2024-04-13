png_set_invalid(png_const_structrp png_ptr, png_inforp info_ptr, int mask)
{
   if (png_ptr != NULL && info_ptr != NULL)
      info_ptr->valid &= ~mask;
}