    template<typename T>
    void _resize(T pixel_type, const unsigned int ndimx, const unsigned int ndimy, const bool force_redraw) {
      Display *const dpy = cimg::X11_attr().display;
      cimg::unused(pixel_type);

#ifdef cimg_use_xshm
      if (_shminfo) {
        XShmSegmentInfo *const nshminfo = new XShmSegmentInfo;
        XImage *const nimage = XShmCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),
                                               cimg::X11_attr().nb_bits,ZPixmap,0,nshminfo,ndimx,ndimy);
        if (!nimage) { delete nshminfo; return; }
        else {
          nshminfo->shmid = shmget(IPC_PRIVATE,ndimx*ndimy*sizeof(T),IPC_CREAT | 0777);
          if (nshminfo->shmid==-1) { XDestroyImage(nimage); delete nshminfo; return; }
          else {
            nshminfo->shmaddr = nimage->data = (char*)shmat(nshminfo->shmid,0,0);
            if (nshminfo->shmaddr==(char*)-1) {
              shmctl(nshminfo->shmid,IPC_RMID,0); XDestroyImage(nimage); delete nshminfo; return;
            } else {
              nshminfo->readOnly = 0;
              cimg::X11_attr().is_shm_enabled = true;
              XErrorHandler oldXErrorHandler = XSetErrorHandler(_assign_xshm);
              XShmAttach(dpy,nshminfo);
              XFlush(dpy);
              XSetErrorHandler(oldXErrorHandler);
              if (!cimg::X11_attr().is_shm_enabled) {
                shmdt(nshminfo->shmaddr);
                shmctl(nshminfo->shmid,IPC_RMID,0);
                XDestroyImage(nimage);
                delete nshminfo;
                return;
              } else {
                T *const ndata = (T*)nimage->data;
                if (force_redraw) _render_resize((T*)_data,_width,_height,ndata,ndimx,ndimy);
                else std::memset(ndata,0,sizeof(T)*ndimx*ndimy);
                XShmDetach(dpy,_shminfo);
                XDestroyImage(_image);
                shmdt(_shminfo->shmaddr);
                shmctl(_shminfo->shmid,IPC_RMID,0);
                delete _shminfo;
                _shminfo = nshminfo;
                _image = nimage;
                _data = (void*)ndata;
              }
            }
          }
        }
      } else
#endif
        {
          T *ndata = (T*)std::malloc(ndimx*ndimy*sizeof(T));
          if (force_redraw) _render_resize((T*)_data,_width,_height,ndata,ndimx,ndimy);
          else std::memset(ndata,0,sizeof(T)*ndimx*ndimy);
          _data = (void*)ndata;
          XDestroyImage(_image);
          _image = XCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),
                                cimg::X11_attr().nb_bits,ZPixmap,0,(char*)_data,ndimx,ndimy,8,0);
        }