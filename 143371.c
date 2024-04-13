
    CImgDisplay& hide_mouse() {
      if (is_empty()) return *this;
      Display *const dpy = cimg::X11_attr().display;
      cimg_lock_display();
      static const char pix_data[8] = { 0 };
      XColor col;
      col.red = col.green = col.blue = 0;
      Pixmap pix = XCreateBitmapFromData(dpy,_window,pix_data,8,8);
      Cursor cur = XCreatePixmapCursor(dpy,pix,pix,&col,&col,0,0);
      XFreePixmap(dpy,pix);
      XDefineCursor(dpy,_window,cur);
      cimg_unlock_display();
      return *this;