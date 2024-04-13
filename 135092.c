bool Matrix::invertTo(Matrix *other) const
{
    const double det_denominator = determinant();
    if (unlikely(det_denominator == 0)) {
        *other = { 1, 0, 0, 1, 0, 0 };
        return false;
    }

    const double det = 1 / det_denominator;
    other->m[0] = m[3] * det;
    other->m[1] = -m[1] * det;
    other->m[2] = -m[2] * det;
    other->m[3] = m[0] * det;
    other->m[4] = (m[2] * m[5] - m[3] * m[4]) * det;
    other->m[5] = (m[1] * m[4] - m[0] * m[5]) * det;

    return true;
}