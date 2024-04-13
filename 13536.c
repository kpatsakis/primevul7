set_ref_in_opfunc(int copyID UNUSED)
{
    int abort = FALSE;

    abort = set_ref_in_callback(&opfunc_cb, copyID);

    return abort;
}