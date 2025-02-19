static MagickBooleanType ReadPSDChannel(Image *image,
  const ImageInfo *image_info,const PSDInfo *psd_info,LayerInfo* layer_info,
  const size_t channel,const PSDCompressionType compression,
  ExceptionInfo *exception)
{
  Image
    *channel_image,
    *mask;

  MagickOffsetType
    offset;

  MagickBooleanType
    status;

  channel_image=image;
  mask=(Image *) NULL;
  if ((layer_info->channel_info[channel].type < -1) &&
      (layer_info->mask.page.width > 0) && (layer_info->mask.page.height > 0))
    {
      const char
        *option;

      /*
        Ignore mask that is not a user supplied layer mask, if the mask is
        disabled or if the flags have unsupported values.
      */
      option=GetImageOption(image_info,"psd:preserve-opacity-mask");
      if ((layer_info->channel_info[channel].type != -2) ||
          (layer_info->mask.flags > 2) || ((layer_info->mask.flags & 0x02) &&
           (IsStringTrue(option) == MagickFalse)))
      {
        (void) SeekBlob(image,(MagickOffsetType)
          (layer_info->channel_info[channel].size-2),SEEK_CUR);
        return(MagickTrue);
      }
      mask=CloneImage(image,layer_info->mask.page.width,
        layer_info->mask.page.height,MagickFalse,exception);
      if (mask != (Image *) NULL)
        {
          (void) ResetImagePixels(mask,exception);
          mask->matte=MagickFalse;
          channel_image=mask;
        }
    }

  offset=TellBlob(image);
  status=MagickFalse;
  switch(compression)
  {
    case Raw:
      status=ReadPSDChannelRaw(channel_image,psd_info->channels,
        (ssize_t) layer_info->channel_info[channel].type,exception);
      break;
    case RLE:
      {
        MagickOffsetType
          *sizes;

        sizes=ReadPSDRLESizes(channel_image,psd_info,channel_image->rows);
        if (sizes == (MagickOffsetType *) NULL)
          ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
            image->filename);
        status=ReadPSDChannelRLE(channel_image,psd_info,
          (ssize_t) layer_info->channel_info[channel].type,sizes,exception);
        sizes=(MagickOffsetType *) RelinquishMagickMemory(sizes);
      }
      break;
    case ZipWithPrediction:
    case ZipWithoutPrediction:
#ifdef MAGICKCORE_ZLIB_DELEGATE
      status=ReadPSDChannelZip(channel_image,layer_info->channels,
        (ssize_t) layer_info->channel_info[channel].type,compression,
        layer_info->channel_info[channel].size-2,exception);
#else
      (void) ThrowMagickException(exception,GetMagickModule(),
          MissingDelegateWarning,"DelegateLibrarySupportNotBuiltIn",
            "'%s' (ZLIB)",image->filename);
#endif
      break;
    default:
      (void) ThrowMagickException(exception,GetMagickModule(),TypeWarning,
        "CompressionNotSupported","'%.20g'",(double) compression);
      break;
  }

  (void) SeekBlob(image,offset+layer_info->channel_info[channel].size-2,
    SEEK_SET);
  if (status == MagickFalse)
    {
      if (mask != (Image *) NULL)
        (void) DestroyImage(mask);
      ThrowBinaryException(CoderError,"UnableToDecompressImage",
        image->filename);
    }
  if (mask != (Image *) NULL)
    {
      if (layer_info->mask.image != (Image *) NULL)
        layer_info->mask.image=DestroyImage(layer_info->mask.image);
      layer_info->mask.image=mask;
    }
  return(status);
}