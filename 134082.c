static int mmac_rotate(const MMacro *mac, unsigned int n)
{
    if (--n < mac->nparam)
        n = (n + mac->rotate) % mac->nparam;

    return n+1;
}