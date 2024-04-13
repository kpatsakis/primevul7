static MagickBooleanType TIFFGetGPSProperties(TIFF *tiff,Image *image,
  ExceptionInfo *exception)
{
#if defined(MAGICKCORE_HAVE_TIFFREADGPSDIRECTORY)
  MagickBooleanType
    status;

  tdir_t
    directory;

#if defined(TIFF_VERSION_BIG)
  uint64
#else
  uint32
#endif
    offset;

  /*
    Read GPS properties.
  */
  offset=0;
  if (TIFFGetField(tiff,TIFFTAG_GPSIFD,&offset) != 1)
    return(MagickFalse);
  directory=TIFFCurrentDirectory(tiff);
  if (TIFFReadGPSDirectory(tiff,offset) != 1)
    {
      TIFFSetDirectory(tiff,directory);
      return(MagickFalse);
    }
  status=TIFFSetImageProperties(tiff,image,"exif:GPS",exception);
  TIFFSetDirectory(tiff,directory);
  return(status);
#else
  magick_unreferenced(tiff);
  magick_unreferenced(image);
  magick_unreferenced(exception);
  return(MagickTrue);
#endif
}