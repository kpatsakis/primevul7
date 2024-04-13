usm_malloc_usmStateReference(void)
{
    struct usmStateReference *retval;

    retval = calloc(1, sizeof(struct usmStateReference));
    if (retval)
        retval->refcnt = 1;

    return retval;
}                               /* end usm_malloc_usmStateReference() */