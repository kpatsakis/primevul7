
    void _assign(const unsigned int dimw, const unsigned int dimh, const char *const ptitle=0,
                 const unsigned int normalization_type=3,
                 const bool fullscreen_flag=false, const bool closed_flag=false) {
      cimg::mutex(14);

      // Allocate space for window title
      const char *const nptitle = ptitle?ptitle:"";
      const unsigned int s = (unsigned int)std::strlen(nptitle) + 1;
      char *const tmp_title = s?new char[s]:0;
      if (s) std::memcpy(tmp_title,nptitle,s*sizeof(char));

      // Destroy previous display window if existing
      if (!is_empty()) assign();

      // Open X11 display and retrieve graphical properties.
      Display* &dpy = cimg::X11_attr().display;
      if (!dpy) {
        dpy = XOpenDisplay(0);
        if (!dpy)
          throw CImgDisplayException(_cimgdisplay_instance
                                     "assign(): Failed to open X11 display.",
                                     cimgdisplay_instance);

        cimg::X11_attr().nb_bits = DefaultDepth(dpy,DefaultScreen(dpy));
        if (cimg::X11_attr().nb_bits!=8 && cimg::X11_attr().nb_bits!=16 &&
            cimg::X11_attr().nb_bits!=24 && cimg::X11_attr().nb_bits!=32)
          throw CImgDisplayException(_cimgdisplay_instance
                                     "assign(): Invalid %u bits screen mode detected "
                                     "(only 8, 16, 24 and 32 bits modes are managed).",
                                     cimgdisplay_instance,
                                     cimg::X11_attr().nb_bits);
        XVisualInfo vtemplate;
        vtemplate.visualid = XVisualIDFromVisual(DefaultVisual(dpy,DefaultScreen(dpy)));
        int nb_visuals;
        XVisualInfo *vinfo = XGetVisualInfo(dpy,VisualIDMask,&vtemplate,&nb_visuals);
        if (vinfo && vinfo->red_mask<vinfo->blue_mask) cimg::X11_attr().is_blue_first = true;
        cimg::X11_attr().byte_order = ImageByteOrder(dpy);
	XFree(vinfo);

        cimg_lock_display();
        cimg::X11_attr().events_thread = new pthread_t;
        pthread_create(cimg::X11_attr().events_thread,0,_events_thread,0);
      } else cimg_lock_display();

      // Set display variables.
      _width = std::min(dimw,(unsigned int)screen_width());
      _height = std::min(dimh,(unsigned int)screen_height());
      _normalization = normalization_type<4?normalization_type:3;
      _is_fullscreen = fullscreen_flag;
      _window_x = _window_y = 0;
      _is_closed = closed_flag;
      _title = tmp_title;
      flush();

      // Create X11 window (and LUT, if 8bits display)
      if (_is_fullscreen) {
        if (!_is_closed) _init_fullscreen();
        const unsigned int sx = screen_width(), sy = screen_height();
        XSetWindowAttributes winattr;
        winattr.override_redirect = 1;
        _window = XCreateWindow(dpy,DefaultRootWindow(dpy),(sx - _width)/2,(sy - _height)/2,_width,_height,0,0,
                                InputOutput,CopyFromParent,CWOverrideRedirect,&winattr);
      } else
        _window = XCreateSimpleWindow(dpy,DefaultRootWindow(dpy),0,0,_width,_height,0,0L,0L);

      XSelectInput(dpy,_window,
		   ExposureMask | StructureNotifyMask | ButtonPressMask | KeyPressMask | PointerMotionMask |
		   EnterWindowMask | LeaveWindowMask | ButtonReleaseMask | KeyReleaseMask);

      XStoreName(dpy,_window,_title?_title:" ");
      if (cimg::X11_attr().nb_bits==8) {
        _colormap = XCreateColormap(dpy,_window,DefaultVisual(dpy,DefaultScreen(dpy)),AllocAll);
        _set_colormap(_colormap,3);
        XSetWindowColormap(dpy,_window,_colormap);
      }

      static const char *const _window_class = cimg_appname;
      XClassHint *const window_class = XAllocClassHint();
      window_class->res_name = (char*)_window_class;
      window_class->res_class = (char*)_window_class;
      XSetClassHint(dpy,_window,window_class);
      XFree(window_class);

      _window_width = _width;
      _window_height = _height;

      // Create XImage
#ifdef cimg_use_xshm
      _shminfo = 0;
      if (XShmQueryExtension(dpy)) {
        _shminfo = new XShmSegmentInfo;
        _image = XShmCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),cimg::X11_attr().nb_bits,
                                 ZPixmap,0,_shminfo,_width,_height);
        if (!_image) { delete _shminfo; _shminfo = 0; }
        else {
          _shminfo->shmid = shmget(IPC_PRIVATE,_image->bytes_per_line*_image->height,IPC_CREAT|0777);
          if (_shminfo->shmid==-1) { XDestroyImage(_image); delete _shminfo; _shminfo = 0; }
          else {
            _shminfo->shmaddr = _image->data = (char*)(_data = shmat(_shminfo->shmid,0,0));
            if (_shminfo->shmaddr==(char*)-1) {
              shmctl(_shminfo->shmid,IPC_RMID,0); XDestroyImage(_image); delete _shminfo; _shminfo = 0;
            } else {
              _shminfo->readOnly = 0;
              cimg::X11_attr().is_shm_enabled = true;
              XErrorHandler oldXErrorHandler = XSetErrorHandler(_assign_xshm);
              XShmAttach(dpy,_shminfo);
              XSync(dpy,0);
              XSetErrorHandler(oldXErrorHandler);
              if (!cimg::X11_attr().is_shm_enabled) {
                shmdt(_shminfo->shmaddr); shmctl(_shminfo->shmid,IPC_RMID,0); XDestroyImage(_image);
                delete _shminfo; _shminfo = 0;
              }
            }
          }
        }
      }
      if (!_shminfo)
#endif
        {
          const cimg_ulong buf_size = (cimg_ulong)_width*_height*(cimg::X11_attr().nb_bits==8?1:
                                                                  (cimg::X11_attr().nb_bits==16?2:4));
          _data = std::malloc(buf_size);
          _image = XCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),cimg::X11_attr().nb_bits,
                                ZPixmap,0,(char*)_data,_width,_height,8,0);
        }

      _wm_window_atom = XInternAtom(dpy,"WM_DELETE_WINDOW",0);
      _wm_protocol_atom = XInternAtom(dpy,"WM_PROTOCOLS",0);
      XSetWMProtocols(dpy,_window,&_wm_window_atom,1);

      if (_is_fullscreen) XGrabKeyboard(dpy,_window,1,GrabModeAsync,GrabModeAsync,CurrentTime);
      cimg::X11_attr().wins[cimg::X11_attr().nb_wins++]=this;
      if (!_is_closed) _map_window(); else { _window_x = _window_y = cimg::type<int>::min(); }
      cimg_unlock_display();
      cimg::mutex(14,0);