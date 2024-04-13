XSetNormalHints (			/* old routine */
	Display *dpy,
	Window w,
	XSizeHints *hints)
{
	return XSetSizeHints (dpy, w, hints, XA_WM_NORMAL_HINTS);
}