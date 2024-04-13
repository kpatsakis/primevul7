
    CImgDisplay& resize(const int nwidth, const int nheight, const bool force_redraw=true) {
      if (!nwidth || !nheight || (is_empty() && (nwidth<0 || nheight<0))) return assign();
      if (is_empty()) return assign(nwidth,nheight);
      const unsigned int
        tmpdimx = (nwidth>0)?nwidth:(-nwidth*_width/100),
        tmpdimy = (nheight>0)?nheight:(-nheight*_height/100),
        dimx = tmpdimx?tmpdimx:1,
        dimy = tmpdimy?tmpdimy:1;
      if (_width!=dimx || _height!=dimy || _window_width!=dimx || _window_height!=dimy) {
        if (_window_width!=dimx || _window_height!=dimy) {
          RECT rect; rect.left = rect.top = 0; rect.right = (LONG)dimx - 1; rect.bottom = (LONG)dimy - 1;
          AdjustWindowRect(&rect,WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,false);
          const int cwidth = rect.right - rect.left + 1, cheight = rect.bottom - rect.top + 1;
          SetWindowPos(_window,0,0,0,cwidth,cheight,SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);
        }
        if (_width!=dimx || _height!=dimy) {
          unsigned int *const ndata = new unsigned int[dimx*dimy];
          if (force_redraw) _render_resize(_data,_width,_height,ndata,dimx,dimy);
          else std::memset(ndata,0x80,sizeof(unsigned int)*dimx*dimy);
          delete[] _data;
          _data = ndata;
          _bmi.bmiHeader.biWidth = (LONG)dimx;
          _bmi.bmiHeader.biHeight = -(int)dimy;
          _width = dimx;
          _height = dimy;
        }
        _window_width = dimx; _window_height = dimy;
        show();
      }
      _is_resized = false;
      if (_is_fullscreen) move((screen_width() - width())/2,(screen_height() - height())/2);
      if (force_redraw) return paint();
      return *this;