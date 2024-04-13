add8x2_8x2 (uint32_t a, uint32_t b)
{
    uint32_t t = a + b;
    t |= RB_MASK_PLUS_ONE - ((t >> G_SHIFT) & RB_MASK);
    return t & RB_MASK;
}