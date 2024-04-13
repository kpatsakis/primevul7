uncompress_image(Gif_Context *gfc, Gif_Image *gfi, Gif_Reader *grr)
{
    int old_nerrors;
    if (!Gif_CreateUncompressedImage(gfi, gfi->interlace))
        return 0;
    gfc->width = gfi->width;
    gfc->height = gfi->height;
    gfc->image = gfi->image_data;
    gfc->maximage = gfi->image_data + (unsigned) gfi->width * (unsigned) gfi->height;
    old_nerrors = gfc->errors[1];
    read_image_data(gfc, grr);
    gfi->compressed_errors = gfc->errors[1] - old_nerrors;
    return 1;
}