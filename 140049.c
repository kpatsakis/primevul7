iaaGetLinearTRC(l_int32  **iaa,
                l_int32    diff)
{
l_int32    i;
l_int32   *ia;
l_float32  factor;

    PROCNAME("iaaGetLinearTRC");

    if (!iaa)
        return (l_int32 *)ERROR_PTR("iaa not defined", procName, NULL);

    if (iaa[diff] != NULL)  /* already have it */
       return iaa[diff];

    if ((ia = (l_int32 *)LEPT_CALLOC(256, sizeof(l_int32))) == NULL)
        return (l_int32 *)ERROR_PTR("ia not made", procName, NULL);
    iaa[diff] = ia;
    if (diff == 0) {  /* shouldn't happen */
        for (i = 0; i < 256; i++)
            ia[i] = 128;
    }
    else {
        factor = 255. / (l_float32)diff;
        for (i = 0; i < diff + 1; i++)
            ia[i] = (l_int32)(factor * i + 0.5);
        for (i = diff + 1; i < 256; i++)
            ia[i] = 255;
    }

    return ia;
}