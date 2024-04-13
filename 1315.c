static MagickBooleanType AnimateUsage(void)
{
  static const char
    buttons[] =
      "  Press any button to map or unmap the Command widget",
    miscellaneous[] =
      "  -debug events        display copious debugging information\n"
      "  -help                print program options\n"
      "  -list type           print a list of supported option arguments\n"
      "  -log format          format of debugging information\n"
      "  -version             print version information",
    operators[] =
      "  -colors value        preferred number of colors in the image\n"
      "  -crop geometry       preferred size and location of the cropped image\n"
      "  -extract geometry    extract area from image\n"
      "  -monochrome          transform image to black and white\n"
      "  -resample geometry   change the resolution of an image\n"
      "  -resize geometry     resize the image\n"
      "  -rotate degrees      apply Paeth rotation to the image\n"
      "  -strip               strip image of all profiles and comments\n"
      "  -thumbnail geometry  create a thumbnail of the image\n"
      "  -trim                trim image edges",
    settings[] =
      "  -alpha option        on, activate, off, deactivate, set, opaque, copy\n"
      "                       transparent, extract, background, or shape\n"
      "  -authenticate password\n"
      "                       decipher image with this password\n"
      "  -backdrop            display image centered on a backdrop\n"
      "  -channel type        apply option to select image channels\n"
      "  -colormap type       Shared or Private\n"
      "  -colorspace type     alternate image colorspace\n"
      "  -decipher filename   convert cipher pixels to plain pixels\n"
      "  -define format:option\n"
      "                       define one or more image format options\n"
      "  -delay value         display the next image after pausing\n"
      "  -density geometry    horizontal and vertical density of the image\n"
      "  -depth value         image depth\n"
      "  -display server      display image to this X server\n"
      "  -dispose method      layer disposal method\n"
      "  -dither method       apply error diffusion to image\n"
      "  -filter type         use this filter when resizing an image\n"
      "  -format \"string\"     output formatted image characteristics\n"
      "  -gamma value         level of gamma correction\n"
      "  -geometry geometry   preferred size and location of the Image window\n"
      "  -gravity type        horizontal and vertical backdrop placement\n"
      "  -identify            identify the format and characteristics of the image\n"
      "  -immutable           displayed image cannot be modified\n"
      "  -interlace type      type of image interlacing scheme\n"
      "  -interpolate method  pixel color interpolation method\n"
      "  -limit type value    pixel cache resource limit\n"
      "  -loop iterations     loop images then exit\n"
      "  -matte               store matte channel if the image has one\n"
      "  -map type            display image using this Standard Colormap\n"
      "  -monitor             monitor progress\n"
      "  -pause               seconds to pause before reanimating\n"
      "  -page geometry       size and location of an image canvas (setting)\n"
      "  -quantize colorspace reduce colors in this colorspace\n"
      "  -quiet               suppress all warning messages\n"
      "  -regard-warnings     pay attention to warning messages\n"
      "  -remote command      execute a command in an remote display process\n"
      "  -repage geometry     size and location of an image canvas (operator)\n"
      "  -respect-parentheses settings remain in effect until parenthesis boundary\n"
      "  -sampling-factor geometry\n"
      "                       horizontal and vertical sampling factor\n"
      "  -scenes range        image scene range\n"
      "  -seed value          seed a new sequence of pseudo-random numbers\n"
      "  -set attribute value set an image attribute\n"
      "  -size geometry       width and height of image\n"
      "  -support factor      resize support: > 1.0 is blurry, < 1.0 is sharp\n"
      "  -transparent-color color\n"
      "                       transparent color\n"
      "  -treedepth value     color tree depth\n"
      "  -verbose             print detailed information about the image\n"
      "  -visual type         display image using this visual type\n"
      "  -virtual-pixel method\n"
      "                       virtual pixel access method\n"
      "  -window id           display image to background of this window",
    sequence_operators[] =
      "  -coalesce            merge a sequence of images\n"
      "  -flatten             flatten a sequence of images";

  ListMagickVersion(stdout);
  (void) printf("Usage: %s [options ...] file [ [options ...] file ...]\n",
    GetClientName());
  (void) printf("\nImage Settings:\n");
  (void) puts(settings);
  (void) printf("\nImage Operators:\n");
  (void) puts(operators);
  (void) printf("\nImage Sequence Operators:\n");
  (void) puts(sequence_operators);
  (void) printf("\nMiscellaneous Options:\n");
  (void) puts(miscellaneous);
  (void) printf(
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) printf(
    "resources as command line options:  -background, -bordercolor,\n");
  (void) printf(
    "-borderwidth, -font, -foreground, -iconGeometry, -iconic, -name,\n");
  (void) printf("-mattecolor, -shared-memory, or -title.\n");
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
  (void) printf("\nButtons: \n");
  (void) puts(buttons);
  return(MagickTrue);
}