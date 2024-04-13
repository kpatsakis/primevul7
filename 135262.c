static inline void GfxDeviceCMYKColorSpacegetRGBLineHelper(unsigned char *&in, double &r, double &g, double &b)
{
    double c, m, y, k, c1, m1, y1, k1;

    c = byteToDbl(*in++);
    m = byteToDbl(*in++);
    y = byteToDbl(*in++);
    k = byteToDbl(*in++);
    c1 = 1 - c;
    m1 = 1 - m;
    y1 = 1 - y;
    k1 = 1 - k;
    cmykToRGBMatrixMultiplication(c, m, y, k, c1, m1, y1, k1, r, g, b);
}