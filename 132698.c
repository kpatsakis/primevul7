_XF86BigfontFreeCodes (
    XExtData *extension)
{
    /* Don't Xfree(extension->private_data) because it is on the same malloc
       chunk as extension. */
    /* Don't Xfree(extension->private_data->codes) because this is shared with
       the display's ext_procs list. */
    return 0;
}