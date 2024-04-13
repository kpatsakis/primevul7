__zzip_aligned4(char *p)
{
#define aligned4   __zzip_aligned4
    p += ((intptr_t) p) & 1;
    p += ((intptr_t) p) & 2;
    return p;
}