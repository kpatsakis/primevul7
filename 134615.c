int LibRaw::dcraw_thumb_writer(const char *fname)
{
//    CHECK_ORDER_LOW(LIBRAW_PROGRESS_THUMB_LOAD);

  if(!fname) 
    return ENOENT;
        
  FILE *tfp = fopen(fname,"wb");
    
  if(!tfp) 
    return errno;

  if(!T.thumb)
    {
      fclose(tfp);
      return LIBRAW_OUT_OF_ORDER_CALL;
    }

  try {
    switch (T.tformat)
      {
      case LIBRAW_THUMBNAIL_JPEG:
        jpeg_thumb_writer (tfp,T.thumb,T.tlength);
        break;
      case LIBRAW_THUMBNAIL_BITMAP:
        fprintf (tfp, "P6\n%d %d\n255\n", T.twidth, T.theight);
        fwrite (T.thumb, 1, T.tlength, tfp);
        break;
      default:
        fclose(tfp);
        return LIBRAW_UNSUPPORTED_THUMBNAIL;
      }
    fclose(tfp);
    return 0;
  }
  catch ( LibRaw_exceptions err) {
    fclose(tfp);
    EXCEPTION_HANDLER(err);
  }
}