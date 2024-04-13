mul8x2_8 (uint32_t a, uint8_t b)
{
    uint32_t t = (a & RB_MASK) * b + RB_ONE_HALF;
    return ((t + ((t >> G_SHIFT) & RB_MASK)) >> G_SHIFT) & RB_MASK;
}