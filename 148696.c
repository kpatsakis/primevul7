stem_hint_handler(void *client_data, gx_san_sect *ss)
{
    t1_hinter_aux *h = (t1_hinter_aux *)client_data;

    if (ss->side_mask == 3) {
        /* Orient horizontal hints to help with top/bottom alignment zones.
           Otherwize glyphs may get a random height due to serif adjustsment. */
        if (ss->xl > h->midx && h->transpose)
            return (h->transpose ? t1_hinter__hstem : t1_hinter__vstem)
                        (&h->super, ss->xr, ss->xl - ss->xr);
        else
            return (h->transpose ? t1_hinter__hstem : t1_hinter__vstem)
                        (&h->super, ss->xl, ss->xr - ss->xl);
    } else
        return t1_hinter__overall_hstem(&h->super, ss->xl, ss->xr - ss->xl, ss->side_mask);
}