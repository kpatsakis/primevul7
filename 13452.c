to_pixman_image (cairo_surface_t *s)
{
    return ((cairo_image_surface_t *)s)->pixman_image;
}