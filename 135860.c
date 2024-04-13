png_set_chunk_malloc_max (png_structrp png_ptr,
    png_alloc_size_t user_chunk_malloc_max)
{
   if (png_ptr != NULL)
      png_ptr->user_chunk_malloc_max = user_chunk_malloc_max;
}