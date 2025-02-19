XSetIconName (
    register Display *dpy,
    Window w,
    _Xconst char *icon_name)
{
    if (strlen(icon_name) >= USHRT_MAX)
        return 0;
    return XChangeProperty(dpy, w, XA_WM_ICON_NAME, XA_STRING, 8,
                           PropModeReplace, (_Xconst unsigned char *)icon_name,
			   icon_name ? (int) strlen(icon_name) : 0);
}