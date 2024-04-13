    template<typename T>
    static void screenshot(const int x0, const int y0, const int x1, const int y1, CImg<T>& img) {
      img.assign();
      Display *dpy = cimg::X11_attr().display;
      cimg_lock_display();
      if (!dpy) {
        dpy = XOpenDisplay(0);
        if (!dpy)
          throw CImgDisplayException("CImgDisplay::screenshot(): Failed to open X11 display.");
      }
      Window root = DefaultRootWindow(dpy);
      XWindowAttributes gwa;
      XGetWindowAttributes(dpy,root,&gwa);
      const int width = gwa.width, height = gwa.height;
      int _x0 = x0, _y0 = y0, _x1 = x1, _y1 = y1;
      if (_x0>_x1) cimg::swap(_x0,_x1);
      if (_y0>_y1) cimg::swap(_y0,_y1);

      XImage *image = 0;
      if (_x1>=0 && _x0<width && _y1>=0 && _y0<height) {
        _x0 = std::max(_x0,0);
        _y0 = std::max(_y0,0);
        _x1 = std::min(_x1,width - 1);
        _y1 = std::min(_y1,height - 1);
        image = XGetImage(dpy,root,_x0,_y0,_x1 - _x0 + 1,_y1 - _y0 + 1,AllPlanes,ZPixmap);

        if (image) {
          const unsigned long
            red_mask = image->red_mask,
            green_mask = image->green_mask,
            blue_mask = image->blue_mask;
          img.assign(image->width,image->height,1,3);
          T *pR = img.data(0,0,0,0), *pG = img.data(0,0,0,1), *pB = img.data(0,0,0,2);
          cimg_forXY(img,x,y) {
            const unsigned long pixel = XGetPixel(image,x,y);
            *(pR++) = (T)((pixel & red_mask)>>16);
            *(pG++) = (T)((pixel & green_mask)>>8);
            *(pB++) = (T)(pixel & blue_mask);
          }
          XDestroyImage(image);
        }
      }
      if (!cimg::X11_attr().display) XCloseDisplay(dpy);
      cimg_unlock_display();
      if (img.is_empty())
        throw CImgDisplayException("CImgDisplay::screenshot(): Failed to take screenshot "
                                   "with coordinates (%d,%d)-(%d,%d).",
                                   x0,y0,x1,y1);