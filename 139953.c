static MagickBooleanType RiemersmaDither(Image *image,CacheView *image_view,
  CubeInfo *cube_info,const unsigned int direction,ExceptionInfo *exception)
{
#define DitherImageTag  "Dither/Image"

  DoublePixelPacket
    color,
    pixel;

  MagickBooleanType
    proceed;

  register CubeInfo
    *p;

  size_t
    index;

  p=cube_info;
  if ((p->x >= 0) && (p->x < (ssize_t) image->columns) &&
      (p->y >= 0) && (p->y < (ssize_t) image->rows))
    {
      register Quantum
        *magick_restrict q;

      register ssize_t
        i;

      /*
        Distribute error.
      */
      q=GetCacheViewAuthenticPixels(image_view,p->x,p->y,1,1,exception);
      if (q == (Quantum *) NULL)
        return(MagickFalse);
      AssociateAlphaPixel(image,cube_info,q,&pixel);
      for (i=0; i < ErrorQueueLength; i++)
      {
        pixel.red+=p->weights[i]*p->error[i].red;
        pixel.green+=p->weights[i]*p->error[i].green;
        pixel.blue+=p->weights[i]*p->error[i].blue;
        if (cube_info->associate_alpha != MagickFalse)
          pixel.alpha+=p->weights[i]*p->error[i].alpha;
      }
      pixel.red=(double) ClampPixel(pixel.red);
      pixel.green=(double) ClampPixel(pixel.green);
      pixel.blue=(double) ClampPixel(pixel.blue);
      if (cube_info->associate_alpha != MagickFalse)
        pixel.alpha=(double) ClampPixel(pixel.alpha);
      i=CacheOffset(cube_info,&pixel);
      if (p->cache[i] < 0)
        {
          register NodeInfo
            *node_info;

          register size_t
            id;

          /*
            Identify the deepest node containing the pixel's color.
          */
          node_info=p->root;
          for (index=MaxTreeDepth-1; (ssize_t) index > 0; index--)
          {
            id=ColorToNodeId(cube_info,&pixel,index);
            if (node_info->child[id] == (NodeInfo *) NULL)
              break;
            node_info=node_info->child[id];
          }
          /*
            Find closest color among siblings and their children.
          */
          p->target=pixel;
          p->distance=(double) (4.0*(QuantumRange+1.0)*((double)
            QuantumRange+1.0)+1.0);
          ClosestColor(image,p,node_info->parent);
          p->cache[i]=(ssize_t) p->color_number;
        }
      /*
        Assign pixel to closest colormap entry.
      */
      index=(size_t) p->cache[i];
      if (image->storage_class == PseudoClass)
        SetPixelIndex(image,(Quantum) index,q);
      if (cube_info->quantize_info->measure_error == MagickFalse)
        {
          SetPixelRed(image,ClampToQuantum(image->colormap[index].red),q);
          SetPixelGreen(image,ClampToQuantum(image->colormap[index].green),q);
          SetPixelBlue(image,ClampToQuantum(image->colormap[index].blue),q);
          if (cube_info->associate_alpha != MagickFalse)
            SetPixelAlpha(image,ClampToQuantum(image->colormap[index].alpha),q);
        }
      if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
        return(MagickFalse);
      /*
        Propagate the error as the last entry of the error queue.
      */
      (void) memmove(p->error,p->error+1,(ErrorQueueLength-1)*
        sizeof(p->error[0]));
      AssociateAlphaPixelInfo(cube_info,image->colormap+index,&color);
      p->error[ErrorQueueLength-1].red=pixel.red-color.red;
      p->error[ErrorQueueLength-1].green=pixel.green-color.green;
      p->error[ErrorQueueLength-1].blue=pixel.blue-color.blue;
      if (cube_info->associate_alpha != MagickFalse)
        p->error[ErrorQueueLength-1].alpha=pixel.alpha-color.alpha;
      proceed=SetImageProgress(image,DitherImageTag,p->offset,p->span);
      if (proceed == MagickFalse)
        return(MagickFalse);
      p->offset++;
    }
  switch (direction)
  {
    case WestGravity: p->x--; break;
    case EastGravity: p->x++; break;
    case NorthGravity: p->y--; break;
    case SouthGravity: p->y++; break;
  }
  return(MagickTrue);
}