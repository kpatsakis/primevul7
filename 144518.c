    **/
    CImgDisplay& set_button(const unsigned int button, const bool is_pressed=true) {
      const unsigned int buttoncode = button==1U?1U:button==2U?2U:button==3U?4U:0U;
      if (is_pressed) _button |= buttoncode; else _button &= ~buttoncode;
      _is_event = buttoncode?true:false;
      if (buttoncode) {
#if cimg_display==1
        pthread_cond_broadcast(&cimg::X11_attr().wait_event);
#elif cimg_display==2
        SetEvent(cimg::Win32_attr().wait_event);
#endif
      }
      return *this;