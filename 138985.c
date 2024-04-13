add_num_buff(buffheader_T *buf, long n)
{
    char_u	number[32];

    sprintf((char *)number, "%ld", n);
    add_buff(buf, number, -1L);
}