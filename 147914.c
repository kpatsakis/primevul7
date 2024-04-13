Xtal( byte bitmask , short d0 , int x , short **bar , short *b1 , short *b2 )
{
        short *a2;

        if (d0 != 0)
                d0 += *b1;

        a2 = *bar + x;

/*dprintf1("[%02X]",*a2);*/
        if (*a2 < d0) {
                d0 -= 16400;
                if (-4096 >= d0) {
                        DifSubK( d0 , b1 , b2 );
                        bld_barrier( bar , x );
                } else {
                        DifSubK( d0 , b1 , b2 );
                }
                return( bitmask );
        } else {
                if (d0 > 56)
                        d0 -= 56;
                DifSubK( d0 , b1 , b2 );
                return( 0 );
        }
}