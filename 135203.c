void Matrix::scale(double sx, double sy)
{
    m[0] *= sx;
    m[1] *= sx;
    m[2] *= sy;
    m[3] *= sy;
}