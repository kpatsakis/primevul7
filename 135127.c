inline void radialCorner2(double x, double y, double a, double &b, double &c, double &d, double dx, double dy, double cr, double inva, double dr, double mindr, bool &valid, double *range)
{
    b = (x)*dx + (y)*dy + cr * dr;
    c = (x) * (x) + (y) * (y)-cr * cr;
    d = b * b - a * c;
    if (d >= 0) {
        double t_corner;

        d = sqrt(d);
        t_corner = (b + d) * inva;
        if (t_corner * dr >= mindr)
            valid = radialExtendRange(range, t_corner, valid);
        t_corner = (b - d) * inva;
        if (t_corner * dr >= mindr)
            valid = radialExtendRange(range, t_corner, valid);
    }
}