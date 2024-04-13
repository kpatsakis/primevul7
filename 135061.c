double Matrix::norm() const
{
    double f, g, h, i, j;

    i = m[0] * m[0] + m[1] * m[1];
    j = m[2] * m[2] + m[3] * m[3];

    f = 0.5 * (i + j);
    g = 0.5 * (i - j);
    h = m[0] * m[2] + m[1] * m[3];

    return sqrt(f + hypot(g, h));
}