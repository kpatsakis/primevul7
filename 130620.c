static inline int u16(const byte *p)
{
        return (p[0] << 8) | p[1];
}