xtal_plane( byte *dp , short *buf[] , byte *oP , short **bar , int plane_size , int xtalbuff_size )
{
        int i;
        int j;
        int x = 0;
        byte bitmask;
        byte out;
        short *p;
        short *b1 , *b2;

        b1 = buf[0];
        b2 = buf[1];
/*
        for ( i = 0 ; i < 100 ; i++ ) {
                dprintf1("[%04X]",bar[0][i]);
        }
        dprintf("\n");
*/
        for ( i = 0 ; i < plane_size ; i++ ) {
                bitmask = 0x80;
                out = 0;
                for ( j = 0 ; j < 8 ; j++ ) {
                        out |= Xtal( bitmask , (short)(*dp) << 6 , x++ , bar , b1++ , b2++ );
                        dp += 4;
                        bitmask >>= 1;
                }
                *oP++ = out;
        }
/*dprintf("\n");*/
        p = buf[0];
/*	dprintf("\n"); */
        buf[0] = buf[1];
        buf[1] = p;

        p = bar[0];
        for ( i = 0 ; i < plane_size*8 ; i++ )
                *p++ = 0;

        /*	memset( p, 0, (xtalbuff_size-16) * W);*/
        p = bar[0];
        for ( i = 0 ; i <= 14 ; i++ )
                bar[i] = bar[i+1];
        bar[15] = p;
}