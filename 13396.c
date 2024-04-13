_pixman_image_add_tristrip (pixman_image_t *image,
			    int dst_x, int dst_y,
			    cairo_tristrip_t *strip)
{
    pixman_triangle_t tri;
    pixman_point_fixed_t *p[3] = {&tri.p1, &tri.p2, &tri.p3 };
    int n;

    set_point (p[0], &strip->points[0]);
    set_point (p[1], &strip->points[1]);
    set_point (p[2], &strip->points[2]);
    pixman_add_triangles (image, -dst_x, -dst_y, 1, &tri);
    for (n = 3; n < strip->num_points; n++) {
	set_point (p[n%3], &strip->points[n]);
	pixman_add_triangles (image, -dst_x, -dst_y, 1, &tri);
    }
}