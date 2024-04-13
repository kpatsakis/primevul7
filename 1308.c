static MagickBooleanType DisplayUsage(void)
{
  static const char
    buttons[] =
      "  1    press to map or unmap the Command widget\n"
      "  2    press and drag to magnify a region of an image\n"
      "  3    press to load an image from a visual image directory",
    miscellaneous[] =
      "  -debug events        display copious debugging information\n"
      "  -help                print program options\n"
      "  -list type           print a list of supported option arguments\n"
      "  -log format          format of debugging information\n"
      "  -version             print version information",
    operators[] =
      "  -auto-orient         automagically orient image\n"
      "  -border geometry     surround image with a border of color\n"
      "  -clip                clip along the first path from the 8BIM profile\n"
      "  -clip-path id        clip along a named path from the 8BIM profile\n"
      "  -colors value        preferred number of colors in the image\n"
      "  -contrast            enhance or reduce the image contrast\n"
      "  -crop geometry       preferred size and location of the cropped image\n"
      "  -decipher filename   convert cipher pixels to plain pixels\n"
      "  -deskew threshold    straighten an image\n"
      "  -despeckle           reduce the speckles within an image\n"
      "  -edge factor         apply a filter to detect edges in the image\n"
      "  -enhance             apply a digital filter to enhance a noisy image\n"
      "  -equalize            perform histogram equalization to an image\n"
      "  -extract geometry    extract area from image\n"
      "  -flip                flip image in the vertical direction\n"
      "  -flop                flop image in the horizontal direction\n"
      "  -frame geometry      surround image with an ornamental border\n"
      "  -fuzz distance       colors within this distance are considered equal\n"
      "  -gamma value         level of gamma correction\n"
      "  -monochrome          transform image to black and white\n"
      "  -negate              replace every pixel with its complementary color\n"
      "  -normalize           transform image to span the full range of colors\n"
      "  -raise value         lighten/darken image edges to create a 3-D effect\n"
      "  -resample geometry   change the resolution of an image\n"
      "  -resize geometry     resize the image\n"
      "  -roll geometry       roll an image vertically or horizontally\n"
      "  -rotate degrees      apply Paeth rotation to the image\n"
      "  -sample geometry     scale image with pixel sampling\n"
      "  -segment value       segment an image\n"
      "  -sharpen geometry    sharpen the image\n"
      "  -strip               strip image of all profiles and comments\n"
      "  -threshold value     threshold the image\n"
      "  -thumbnail geometry  create a thumbnail of the image\n"
      "  -trim                trim image edges",
    settings[] =
      "  -alpha option        on, activate, off, deactivate, set, opaque, copy\n"
      "                       transparent, extract, background, or shape\n"
      "  -antialias           remove pixel-aliasing\n"
      "  -authenticate password\n"
      "                       decipher image with this password\n"
      "  -backdrop            display image centered on a backdrop\n"
      "  -channel type        apply option to select image channels\n"
      "  -colormap type       Shared or Private\n"
      "  -colorspace type     alternate image colorspace\n"
      "  -comment string      annotate image with comment\n"
      "  -compress type       type of pixel compression when writing the image\n"
      "  -define format:option\n"
      "                       define one or more image format options\n"
      "  -delay value         display the next image after pausing\n"
      "  -density geometry    horizontal and vertical density of the image\n"
      "  -depth value         image depth\n"
      "  -display server      display image to this X server\n"
      "  -dispose method      layer disposal method\n"
      "  -dither method       apply error diffusion to image\n"
      "  -endian type         endianness (MSB or LSB) of the image\n"
      "  -filter type         use this filter when resizing an image\n"
      "  -format string     output formatted image characteristics\n"
      "  -geometry geometry   preferred size and location of the Image window\n"
      "  -gravity type        horizontal and vertical backdrop placement\n"
      "  -identify            identify the format and characteristics of the image\n"
      "  -immutable           displayed image cannot be modified\n"
      "  -interlace type      type of image interlacing scheme\n"
      "  -interpolate method  pixel color interpolation method\n"
      "  -label string        assign a label to an image\n"
      "  -limit type value    pixel cache resource limit\n"
      "  -loop iterations     loop images then exit\n"
      "  -map type            display image using this Standard Colormap\n"
      "  -matte               store matte channel if the image has one\n"
      "  -monitor             monitor progress\n"
      "  -nostdin             do not try to open stdin\n"
      "  -page geometry       size and location of an image canvas\n"
      "  -profile filename    add, delete, or apply an image profile\n"
      "  -quality value       JPEG/MIFF/PNG compression level\n"
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
      "  -set property value  set an image property\n"
      "  -size geometry       width and height of image\n"
      "  -support factor      resize support: > 1.0 is blurry, < 1.0 is sharp\n"
      "  -texture filename    name of texture to tile onto the image background\n"
      "  -transparent-color color\n"
      "                       transparent color\n"
      "  -treedepth value     color tree depth\n"
      "  -update seconds      detect when image file is modified and redisplay\n"
      "  -verbose             print detailed information about the image\n"
      "  -visual type         display image using this visual type\n"
      "  -virtual-pixel method\n"
      "                       virtual pixel access method\n"
      "  -window id           display image to background of this window\n"
      "  -window-group id     exit program when this window id is destroyed\n"
      "  -write filename      write image to a file",
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
    "-borderwidth, -font, -foreground, -iconGeometry, -iconic, -mattecolor,\n");
  (void) printf("-name, -shared-memory, -usePixmap, or -title.\n");
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