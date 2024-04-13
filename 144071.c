    **/
    CImgDisplay& set_wheel(const int amplitude) {
      _wheel+=amplitude;
      _is_event = amplitude?true:false;
      if (amplitude) {
#if cimg_display==1
        pthread_cond_broadcast(&cimg::X11_attr().wait_event);
#elif cimg_display==2
        SetEvent(cimg::Win32_attr().wait_event);
#endif
      }
      return *this;