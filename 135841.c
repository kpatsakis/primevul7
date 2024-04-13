png_set_chunk_cache_max (png_structrp png_ptr, png_uint_32 user_chunk_cache_max)
{
   if (png_ptr != NULL)
      png_ptr->user_chunk_cache_max = user_chunk_cache_max;
}