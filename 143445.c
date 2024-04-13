
    static int _assign_xshm(Display *dpy, XErrorEvent *error) {
      cimg::unused(dpy,error);
      cimg::X11_attr().is_shm_enabled = false;
      return 0;