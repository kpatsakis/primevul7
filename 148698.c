transpose_path(gx_path *path)
{   segment *s = (segment *)path->first_subpath;

    exch(path->bbox.p.x, path->bbox.p.y);
    exch(path->bbox.q.x, path->bbox.q.y);
    for (; s; s = s->next) {
        if (s->type == s_curve) {
            curve_segment *c = (curve_segment *)s;

            exch(c->p1.x, c->p1.y);
            exch(c->p2.x, c->p2.y);
        }
        exch(s->pt.x, s->pt.y);
    }
}