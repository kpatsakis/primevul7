static void TIFFIgnoreTags(TIFF *tiff)
{
  char
    *q;

  const char
    *p,
    *tags;

  Image
   *image;

  register ssize_t
    i;

  size_t
    count;

  TIFFFieldInfo
    *ignore;

  if (TIFFGetReadProc(tiff) != TIFFReadBlob)
    return;
  image=(Image *)TIFFClientdata(tiff);
  tags=GetImageArtifact(image,"tiff:ignore-tags");
  if (tags == (const char *) NULL)
    return;
  count=0;
  p=tags;
  while (*p != '\0')
  {
    while ((isspace((int) ((unsigned char) *p)) != 0))
      p++;

    (void) strtol(p,&q,10);
    if (p == q)
      return;

    p=q;
    count++;

    while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
      p++;
  }
  if (count == 0)
    return;
  i=0;
  p=tags;
  ignore=(TIFFFieldInfo *) AcquireQuantumMemory(count,sizeof(*ignore));
  if (ignore == (TIFFFieldInfo *) NULL)
    return;
  /* This also sets field_bit to 0 (FIELD_IGNORE) */
  memset(ignore,0,count*sizeof(*ignore));
  while (*p != '\0')
  {
    while ((isspace((int) ((unsigned char) *p)) != 0))
      p++;

    ignore[i].field_tag=(ttag_t) strtol(p,&q,10);

    p=q;
    i++;

    while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
      p++;
  }
  (void) TIFFMergeFieldInfo(tiff,ignore,(uint32) count);
  ignore=(TIFFFieldInfo *) RelinquishMagickMemory(ignore);
}