XFreeFont(
    register Display *dpy,
    XFontStruct *fs)
{
    register xResourceReq *req;
    register _XExtension *ext;

    LockDisplay(dpy);
    /* call out to any extensions interested */
    for (ext = dpy->ext_procs; ext; ext = ext->next)
	if (ext->free_Font) (*ext->free_Font)(dpy, fs, &ext->codes);
    GetResReq (CloseFont, fs->fid, req);
    UnlockDisplay(dpy);
    SyncHandle();
    if (fs->per_char) {
#ifdef USE_XF86BIGFONT
	_XF86BigfontFreeFontMetrics(fs);
#else
	Xfree (fs->per_char);
#endif
    }
    _XFreeExtData(fs->ext_data);

    Xfree (fs->properties);
    Xfree (fs);
    return 1;
}