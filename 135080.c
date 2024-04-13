void Matrix::translate(double tx, double ty)
{
    double x0 = tx * m[0] + ty * m[2] + m[4];
    double y0 = tx * m[1] + ty * m[3] + m[5];
    m[4] = x0;
    m[5] = y0;
}