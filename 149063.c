ModuleExport size_t RegisterSVGImage(void)
{
  char
    version[MagickPathExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(LIBXML_DOTTED_VERSION)
  (void) CopyMagickString(version,"XML " LIBXML_DOTTED_VERSION,
    MagickPathExtent);
#endif
#if defined(MAGICKCORE_RSVG_DELEGATE)
#if !GLIB_CHECK_VERSION(2,35,0)
  g_type_init();
#endif
  (void) FormatLocaleString(version,MagickPathExtent,"RSVG %d.%d.%d",
    LIBRSVG_MAJOR_VERSION,LIBRSVG_MINOR_VERSION,LIBRSVG_MICRO_VERSION);
#endif
  entry=AcquireMagickInfo("SVG","SVG","Scalable Vector Graphics");
  entry->decoder=(DecodeImageHandler *) ReadSVGImage;
  entry->encoder=(EncodeImageHandler *) WriteSVGImage;
  entry->flags^=CoderBlobSupportFlag;
#if defined(MAGICKCORE_RSVG_DELEGATE)
  entry->flags^=CoderDecoderThreadSupportFlag;
#endif
  entry->mime_type=ConstantString("image/svg+xml");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->magick=(IsImageFormatHandler *) IsSVG;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("SVG","SVGZ","Compressed Scalable Vector Graphics");
#if defined(MAGICKCORE_XML_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadSVGImage;
#endif
  entry->encoder=(EncodeImageHandler *) WriteSVGImage;
  entry->flags^=CoderBlobSupportFlag;
#if defined(MAGICKCORE_RSVG_DELEGATE)
  entry->flags^=CoderDecoderThreadSupportFlag;
#endif
  entry->mime_type=ConstantString("image/svg+xml");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->magick=(IsImageFormatHandler *) IsSVG;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("SVG","MSVG",
    "ImageMagick's own SVG internal renderer");
#if defined(MAGICKCORE_XML_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadSVGImage;
#endif
  entry->encoder=(EncodeImageHandler *) WriteSVGImage;
  entry->flags^=CoderBlobSupportFlag;
#if defined(MAGICKCORE_RSVG_DELEGATE)
  entry->flags^=CoderDecoderThreadSupportFlag;
#endif
  entry->magick=(IsImageFormatHandler *) IsSVG;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}