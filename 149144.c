static int PNGSetExifProfile(Image *image,png_size_t size,png_byte *data,
  ExceptionInfo *exception)
{
  StringInfo
    *profile;

  unsigned char
    *p;

  png_byte
    *s;

  size_t
    i;

  profile=BlobToStringInfo((const void *) NULL,size+6);

  if (profile == (StringInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",
        image->filename);
      return(-1);
    }
  p=GetStringInfoDatum(profile);

  /* Initialize profile with "Exif\0\0" */
  *p++ ='E';
  *p++ ='x';
  *p++ ='i';
  *p++ ='f';
  *p++ ='\0';
  *p++ ='\0';

  s=data;
  i=0;
  if (size > 6)
    {
      /* Skip first 6 bytes if "Exif\0\0" is
          already present by accident
      */
      if (s[0] == 'E' && s[1] == 'x'  && s[2] == 'i' &&
          s[3] == 'f' && s[4] == '\0' && s[5] == '\0')
      {
        s+=6;
        i=6;
        SetStringInfoLength(profile,size);
        p=GetStringInfoDatum(profile);
      }
    }

  /* copy chunk->data to profile */
  for (; i<size; i++)
    *p++ = *s++;

  (void) SetImageProfile(image,"exif",profile,exception);

  profile=DestroyStringInfo(profile);

  return(1);
}