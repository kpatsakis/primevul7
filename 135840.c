png_get_int_32)(png_const_bytep buf)
{
   png_uint_32 uval = png_get_uint_32(buf);
   if ((uval & 0x80000000) == 0) /* non-negative */
      return uval;

   uval = (uval ^ 0xffffffff) + 1;  /* 2's complement: -x = ~x+1 */
   if ((uval & 0x80000000) == 0) /* no overflow */
       return -(png_int_32)uval;
   /* The following has to be safe; this function only gets called on PNG data
    * and if we get here that data is invalid.  0 is the most safe value and
    * if not then an attacker would surely just generate a PNG with 0 instead.
    */
   return 0;
}