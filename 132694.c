XStoreName (
    register Display *dpy,
    Window w,
    _Xconst char *name)
{
    if (strlen(name) >= USHRT_MAX)
        return 0;
    return XChangeProperty(dpy, w, XA_WM_NAME, XA_STRING, /*  */
			   8, PropModeReplace, (_Xconst unsigned char *)name,
			   name ? (int) strlen(name) : 0);
}