XFontStruct *XQueryFont (
    register Display *dpy,
    Font fid)
{
    XFontStruct *font_result;
#ifdef USE_XF86BIGFONT
    XF86BigfontCodes *extcodes = _XF86BigfontCodes(dpy);
#endif

    LockDisplay(dpy);
    font_result = NULL;
#ifdef USE_XF86BIGFONT
    if (extcodes) {
	font_result = _XF86BigfontQueryFont(dpy, extcodes, fid, 0L);
    }
#endif
    if (!font_result)
	font_result = _XQueryFont(dpy, fid, 0L);
    UnlockDisplay(dpy);
    SyncHandle();
    return font_result;
}