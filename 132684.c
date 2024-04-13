XSetTransientForHint(
	Display *dpy,
	Window w,
	Window propWindow)
{
	return XChangeProperty(dpy, w, XA_WM_TRANSIENT_FOR, XA_WINDOW, 32,
			       PropModeReplace, (unsigned char *) &propWindow, 1);
}