path_to_hinter(t1_hinter *h, gx_path *path)
{   int code;
    gs_path_enum penum;
    gs_fixed_point pts[3];
    gs_fixed_point p = {0, 0}; /* initialize to avoid a warning */
    bool first = true;
    int op;

    code = gx_path_enum_init(&penum, path);
    if (code < 0)
        return code;
    while ((op = gx_path_enum_next(&penum, pts)) != 0) {
        switch (op) {
            case gs_pe_moveto:
                if (first) {
                    first = false;
                    p = pts[0];
                    code = t1_hinter__rmoveto(h, p.x, p.y);
                } else
                    code = t1_hinter__rmoveto(h, pts[0].x - p.x, pts[0].y - p.y);
                break;
            case gs_pe_lineto:
            case gs_pe_gapto:
                code = t1_hinter__rlineto(h, pts[0].x - p.x, pts[0].y - p.y);
                break;
            case gs_pe_curveto:
                code = t1_hinter__rcurveto(h, pts[0].x - p.x, pts[0].y - p.y,
                                        pts[1].x - pts[0].x, pts[1].y - pts[0].y,
                                        pts[2].x - pts[1].x, pts[2].y - pts[1].y);
                pts[0] = pts[2];
                break;
            case gs_pe_closepath:
                code = t1_hinter__closepath(h);
                break;
            default:
                return_error(gs_error_unregistered);
        }
        if (code < 0)
            return code;
        p = pts[0];
    }
    return 0;
}