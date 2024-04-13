static void read_eXIf_chunk(Image *image,png_struct *ping,png_info *info,
  ExceptionInfo *exception)
{
  png_uint_32
    size;

  png_bytep
    data;

#if PNG_LIBPNG_VER > 10631
  if (png_get_eXIf_1(ping,info,&size,&data))
    (void) PNGSetExifProfile(image,size,data,exception);
#endif
}