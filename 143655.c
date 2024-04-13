
    void _init_fullscreen() {
      _background_window = 0;
      if (!_is_fullscreen || _is_closed) _curr_mode.dmSize = 0;
      else {
        DEVMODE mode;
        unsigned int imode = 0, ibest = 0, bestbpp = 0, bw = ~0U, bh = ~0U;
        for (mode.dmSize = sizeof(DEVMODE), mode.dmDriverExtra = 0; EnumDisplaySettings(0,imode,&mode); ++imode) {
          const unsigned int nw = mode.dmPelsWidth, nh = mode.dmPelsHeight;
          if (nw>=_width && nh>=_height && mode.dmBitsPerPel>=bestbpp && nw<=bw && nh<=bh) {
            bestbpp = mode.dmBitsPerPel;
            ibest = imode;
            bw = nw; bh = nh;
          }
        }
        if (bestbpp) {
          _curr_mode.dmSize = sizeof(DEVMODE); _curr_mode.dmDriverExtra = 0;
          EnumDisplaySettings(0,ENUM_CURRENT_SETTINGS,&_curr_mode);
          EnumDisplaySettings(0,ibest,&mode);
          ChangeDisplaySettings(&mode,0);
        } else _curr_mode.dmSize = 0;

        const unsigned int
          sx = (unsigned int)screen_width(),
          sy = (unsigned int)screen_height();
        if (sx!=_width || sy!=_height) {
          CLIENTCREATESTRUCT background_ccs;
          _background_window = CreateWindowA("MDICLIENT","",WS_POPUP | WS_VISIBLE, 0,0,sx,sy,0,0,0,&background_ccs);
          SetForegroundWindow(_background_window);
        }
      }