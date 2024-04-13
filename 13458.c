set_point (pixman_point_fixed_t *p, cairo_point_t *c)
{
    p->x = _cairo_fixed_to_16_16 (c->x);
    p->y = _cairo_fixed_to_16_16 (c->y);
}