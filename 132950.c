static Image *ReadXPMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    *grey,
    key[MagickPathExtent],
    target[MagickPathExtent],
    *xpm_buffer;

  Image
    *image;

  MagickBooleanType
    active,
    status;

  char
    *next,
    *p,
    *q;

  Quantum
    *r;

  ssize_t
    x;

  size_t
    length;

  SplayTreeInfo
    *xpm_colors;

  ssize_t
    count,
    j,
    y;

  unsigned long
    colors,
    columns,
    rows,
    width;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read XPM file.
  */
  length=MagickPathExtent;
  xpm_buffer=(char *) AcquireQuantumMemory((size_t) length,sizeof(*xpm_buffer));
  if (xpm_buffer == (char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  *xpm_buffer='\0';
  p=xpm_buffer;
  while (ReadBlobString(image,p) != (char *) NULL)
  {
    if ((*p == '#') && ((p == xpm_buffer) || (*(p-1) == '\n')))
      continue;
    if ((*p == '}') && (*(p+1) == ';'))
      break;
    p+=strlen(p);
    if ((size_t) (p-xpm_buffer+MagickPathExtent) < length)
      continue;
    length<<=1;
    xpm_buffer=(char *) ResizeQuantumMemory(xpm_buffer,length+MagickPathExtent,
      sizeof(*xpm_buffer));
    if (xpm_buffer == (char *) NULL)
      break;
    p=xpm_buffer+strlen(xpm_buffer);
  }
  if (xpm_buffer == (char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Remove comments.
  */
  count=0;
  width=0;
  for (p=xpm_buffer; *p != '\0'; p++)
  {
    if (*p != '"')
      continue;
    count=(ssize_t) sscanf(p+1,"%lu %lu %lu %lu",&columns,&rows,&colors,&width);
    image->columns=columns;
    image->rows=rows;
    image->colors=colors;
    if (count == 4)
      break;
  }
  if ((count != 4) || (width == 0) || (width > 3) ||
      (image->columns == 0) || (image->rows == 0) ||
      (image->colors == 0) || (image->colors > MaxColormapSize))
    {
      xpm_buffer=DestroyString(xpm_buffer);
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    }
  /*
    Remove unquoted characters.
  */
  active=MagickFalse;
  for (q=xpm_buffer; *p != '\0'; )
  {
    if (*p++ == '"')
      {
        if (active != MagickFalse)
          *q++='\n';
        active=active != MagickFalse ? MagickFalse : MagickTrue;
      }
    if (active != MagickFalse)
      *q++=(*p);
  }
  *q='\0';
  if (active != MagickFalse)
    {
      xpm_buffer=DestroyString(xpm_buffer);
      ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
    }
  /*
    Initialize image structure.
  */
  xpm_colors=NewSplayTree(CompareXPMColor,RelinquishMagickMemory,
    (void *(*)(void *)) NULL);
  if (AcquireImageColormap(image,image->colors,exception) == MagickFalse)
    {
      xpm_colors=DestroySplayTree(xpm_colors);
      xpm_buffer=DestroyString(xpm_buffer);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  /*
    Read image colormap.
  */
  image->depth=1;
  next=NextXPMLine(xpm_buffer);
  for (j=0; (j < (ssize_t) image->colors) && (next != (char *) NULL); j++)
  {
    char
      symbolic[MagickPathExtent];

    p=next;
    next=NextXPMLine(p);
    if (next == (char *) NULL)
      break;
    length=MagickMin((size_t) width,MagickPathExtent-1);
    if (CopyXPMColor(key,p,length) != (ssize_t) length)
      break;
    status=AddValueToSplayTree(xpm_colors,ConstantString(key),(void *) j);
    /*
      Parse color.
    */
    (void) memset(target,0,sizeof(target));
    (void) CopyMagickString(target,"gray",MagickPathExtent);
    q=(char *) NULL;
    if (strlen(p) > width)
      q=ParseXPMColor(p+width,MagickTrue);
    (void) memset(symbolic,0,sizeof(symbolic));
    *symbolic='\0';
    if (q != (char *) NULL)
      {
        while ((isspace((int) ((unsigned char) *q)) == 0) && (*q != '\0'))
          q++;
        if ((next-q) < 0)
          break;
        (void) CopyXPMColor(target,q,MagickMin((size_t) (next-q),
          MagickPathExtent-1));
        q=ParseXPMColor(target,MagickFalse);
        (void) CopyXPMColor(symbolic,q,MagickMin((size_t) (next-q),
          MagickPathExtent-1));
        if (q != (char *) NULL)
          *q='\0';
      }
    (void) StripMagickString(target);
    if (*symbolic != '\0')
      (void) AddValueToSplayTree(xpm_symbolic,ConstantString(target),
        ConstantString(symbolic));
    grey=strstr(target,"grey");
    if (grey != (char *) NULL)
      grey[2]='a';
    if (LocaleCompare(target,"none") == 0)
      {
        image->storage_class=DirectClass;
        image->alpha_trait=BlendPixelTrait;
      }
    status=QueryColorCompliance(target,XPMCompliance,&image->colormap[j],
      exception);
    if (status == MagickFalse)
      break;
    if (image->depth < image->colormap[j].depth)
      image->depth=image->colormap[j].depth;
  }
  if (j < (ssize_t) image->colors)
    {
      xpm_colors=DestroySplayTree(xpm_colors);
      xpm_buffer=DestroyString(xpm_buffer);
      ThrowReaderException(CorruptImageError,"CorruptImage");
    }
  j=0;
  if (image_info->ping == MagickFalse)
    {
      /*
        Read image pixels.
      */
      status=SetImageExtent(image,image->columns,image->rows,exception);
      if (status == MagickFalse)
        {
          xpm_colors=DestroySplayTree(xpm_colors);
          xpm_buffer=DestroyString(xpm_buffer);
          return(DestroyImageList(image));
        }
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        p=NextXPMLine(p);
        if (p == (char *) NULL)
          break;
        r=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (r == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          count=CopyXPMColor(key,p,MagickMin(width,MagickPathExtent-1));
          if (count != (ssize_t) width)
            break;
          j=(ssize_t) GetValueFromSplayTree(xpm_colors,key);
          if (image->storage_class == PseudoClass)
            SetPixelIndex(image,(Quantum) j,r);
          SetPixelViaPixelInfo(image,image->colormap+j,r);
          p+=count;
          r+=GetPixelChannels(image);
        }
        if (x < (ssize_t) image->columns)
          break;
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      if (y < (ssize_t) image->rows)
        {
          xpm_colors=DestroySplayTree(xpm_colors);
          xpm_buffer=DestroyString(xpm_buffer);
          ThrowReaderException(CorruptImageError,"NotEnoughPixelData");
        }
    }
  /*
    Relinquish resources.
  */
  xpm_buffer=DestroyString(xpm_buffer);
  xpm_colors=DestroySplayTree(xpm_colors);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}