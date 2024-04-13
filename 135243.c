inline void radialEdge(double num, double den, double delta, double lower, double upper, double dr, double mindr, bool &valid, double *range)
{
    if (fabs(den) >= RADIAL_EPSILON) {
        double t_edge, v;
        t_edge = (num) / (den);
        v = t_edge * (delta);
        if (t_edge * dr >= mindr && (lower) <= v && v <= (upper))
            valid = radialExtendRange(range, t_edge, valid);
    }
}