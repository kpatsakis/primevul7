MagickExport ImageInfo *CloneImageInfo(const ImageInfo *image_info)
{
  ImageInfo
    *clone_info;

  clone_info=AcquireImageInfo();
  if (image_info == (ImageInfo *) NULL)
    return(clone_info);
  clone_info->compression=image_info->compression;
  clone_info->temporary=image_info->temporary;
  clone_info->adjoin=image_info->adjoin;
  clone_info->antialias=image_info->antialias;
  clone_info->scene=image_info->scene;
  clone_info->number_scenes=image_info->number_scenes;
  clone_info->depth=image_info->depth;
  if (image_info->size != (char *) NULL)
    (void) CloneString(&clone_info->size,image_info->size);
  if (image_info->extract != (char *) NULL)
    (void) CloneString(&clone_info->extract,image_info->extract);
  if (image_info->scenes != (char *) NULL)
    (void) CloneString(&clone_info->scenes,image_info->scenes);
  if (image_info->page != (char *) NULL)
    (void) CloneString(&clone_info->page,image_info->page);
  clone_info->interlace=image_info->interlace;
  clone_info->endian=image_info->endian;
  clone_info->units=image_info->units;
  clone_info->quality=image_info->quality;
  if (image_info->sampling_factor != (char *) NULL)
    (void) CloneString(&clone_info->sampling_factor,
      image_info->sampling_factor);
  if (image_info->server_name != (char *) NULL)
    (void) CloneString(&clone_info->server_name,image_info->server_name);
  if (image_info->font != (char *) NULL)
    (void) CloneString(&clone_info->font,image_info->font);
  if (image_info->texture != (char *) NULL)
    (void) CloneString(&clone_info->texture,image_info->texture);
  if (image_info->density != (char *) NULL)
    (void) CloneString(&clone_info->density,image_info->density);
  clone_info->pointsize=image_info->pointsize;
  clone_info->fuzz=image_info->fuzz;
  clone_info->matte_color=image_info->matte_color;
  clone_info->background_color=image_info->background_color;
  clone_info->border_color=image_info->border_color;
  clone_info->transparent_color=image_info->transparent_color;
  clone_info->dither=image_info->dither;
  clone_info->monochrome=image_info->monochrome;
  clone_info->colorspace=image_info->colorspace;
  clone_info->type=image_info->type;
  clone_info->orientation=image_info->orientation;
  clone_info->ping=image_info->ping;
  clone_info->verbose=image_info->verbose;
  clone_info->progress_monitor=image_info->progress_monitor;
  clone_info->client_data=image_info->client_data;
  clone_info->cache=image_info->cache;
  if (image_info->cache != (void *) NULL)
    clone_info->cache=ReferencePixelCache(image_info->cache);
  if (image_info->profile != (void *) NULL)
    clone_info->profile=(void *) CloneStringInfo((StringInfo *)
      image_info->profile);
  SetImageInfoFile(clone_info,image_info->file);
  SetImageInfoBlob(clone_info,image_info->blob,image_info->length);
  clone_info->stream=image_info->stream;
  clone_info->custom_stream=image_info->custom_stream;
  (void) CopyMagickString(clone_info->magick,image_info->magick,
    MagickPathExtent);
  (void) CopyMagickString(clone_info->unique,image_info->unique,
    MagickPathExtent);
  (void) CopyMagickString(clone_info->filename,image_info->filename,
    MagickPathExtent);
  clone_info->channel=image_info->channel;
  (void) CloneImageOptions(clone_info,image_info);
  clone_info->debug=IsEventLogging();
  clone_info->signature=image_info->signature;
  return(clone_info);
}