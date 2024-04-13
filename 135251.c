inline void radialCorner1(double x, double y, double &b, double dx, double dy, double cr, double dr, double mindr, bool &valid, double *range)
{
    b = (x)*dx + (y)*dy + cr * dr;
    if (fabs(b) >= RADIAL_EPSILON) {
        double t_corner;
        double x2 = (x) * (x);
        double y2 = (y) * (y);
        double cr2 = (cr) * (cr);
        double c = x2 + y2 - cr2;

        t_corner = 0.5 * c / b;
        if (t_corner * dr >= mindr)
            valid = radialExtendRange(range, t_corner, valid);
    }
}