emergency_print(u_char * field, u_int length)
{
    int             iindex;
    int             start = 0;
    int             stop = 25;

    while (start < stop) {
        for (iindex = start; iindex < stop; iindex++)
            printf("%02X ", field[iindex]);

        printf("\n");
        start = stop;
        stop = stop + 25 < length ? stop + 25 : length;
    }
    fflush(0);

}                               /* end emergency_print() */