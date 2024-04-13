static void TIFFSetProperties(TIFF *tiff,const ImageInfo *image_info,
  Image *image)
{
  const char
    *value;

  value=GetImageArtifact(image,"tiff:document");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_DOCUMENTNAME,value);
  value=GetImageArtifact(image,"tiff:hostcomputer");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_HOSTCOMPUTER,value);
  value=GetImageArtifact(image,"tiff:artist");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_ARTIST,value);
  value=GetImageArtifact(image,"tiff:timestamp");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_DATETIME,value);
  value=GetImageArtifact(image,"tiff:make");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_MAKE,value);
  value=GetImageArtifact(image,"tiff:model");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_MODEL,value);
  value=GetImageArtifact(image,"tiff:software");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_SOFTWARE,value);
  value=GetImageArtifact(image,"tiff:copyright");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_COPYRIGHT,value);
  value=GetImageArtifact(image,"kodak-33423");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,33423,value);
  value=GetImageArtifact(image,"kodak-36867");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,36867,value);
  value=GetImageProperty(image,"label");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_PAGENAME,value);
  value=GetImageProperty(image,"comment");
  if (value != (const char *) NULL)
    (void) TIFFSetField(tiff,TIFFTAG_IMAGEDESCRIPTION,value);
  value=GetImageArtifact(image,"tiff:subfiletype");
  if (value != (const char *) NULL)
    {
      if (LocaleCompare(value,"REDUCEDIMAGE") == 0)
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_REDUCEDIMAGE);
      else
        if (LocaleCompare(value,"PAGE") == 0)
          (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
        else
          if (LocaleCompare(value,"MASK") == 0)
            (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_MASK);
    }
  else
    {
      uint16
        page,
        pages;

      page=(uint16) image->scene;
      pages=(uint16) GetImageListLength(image);
      if ((image_info->adjoin != MagickFalse) && (pages > 1))
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
      (void) TIFFSetField(tiff,TIFFTAG_PAGENUMBER,page,pages);
    }
}