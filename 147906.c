bld_barrier( short **bar , int x )
{
        int i , j;

        short t;
        short *p;
        short *b;
        short *dat = barrier_dat + 1;

        p = *bar++ + x + 1;

        for ( i = 0 ; i < 11 ; i++ ) {
                t = *dat++;
                if (*p < t )
                        *p = t;
                p++;
        }

        for ( j = 0 ; j < 11 ; j++ ) {
                p = *bar++ + x;
                b = p;

                t = *dat++;
                if (*p < t )
                        *p = t;
                p++;
                for ( i = 0 ; i < 11 ; i++ ) {
                        t = *dat++;
                        if (*p < t )
                                *p = t;
                        p++;

                        if (*(--b) < t )
                                *b = t;
                }
        }
}