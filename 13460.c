lerp8x4 (uint32_t src, uint8_t a, uint32_t dst)
{
    return (add8x2_8x2 (mul8x2_8 (src, a),
			mul8x2_8 (dst, ~a)) |
	    add8x2_8x2 (mul8x2_8 (src >> G_SHIFT, a),
			mul8x2_8 (dst >> G_SHIFT, ~a)) << G_SHIFT);
}