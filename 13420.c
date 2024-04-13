mul8_8 (uint8_t a, uint8_t b)
{
    uint16_t t = a * (uint16_t)b + ONE_HALF;
    return ((t >> G_SHIFT) + t) >> G_SHIFT;
}