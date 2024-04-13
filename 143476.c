    template<typename T>
    static void screenshot(const int x0, const int y0, const int x1, const int y1, CImg<T>& img) {
      img.assign();
      HDC hScreen = GetDC(GetDesktopWindow());
      if (hScreen) {
        const int
          width = GetDeviceCaps(hScreen,HORZRES),
          height = GetDeviceCaps(hScreen,VERTRES);
        int _x0 = x0, _y0 = y0, _x1 = x1, _y1 = y1;
        if (_x0>_x1) cimg::swap(_x0,_x1);
        if (_y0>_y1) cimg::swap(_y0,_y1);
        if (_x1>=0 && _x0<width && _y1>=0 && _y0<height) {
          _x0 = std::max(_x0,0);
          _y0 = std::max(_y0,0);
          _x1 = std::min(_x1,width - 1);
          _y1 = std::min(_y1,height - 1);
          const int bw = _x1 - _x0 + 1, bh = _y1 - _y0 + 1;
          HDC hdcMem = CreateCompatibleDC(hScreen);
          if (hdcMem) {
            HBITMAP hBitmap = CreateCompatibleBitmap(hScreen,bw,bh);
            if (hBitmap) {
              HGDIOBJ hOld = SelectObject(hdcMem,hBitmap);
              if (hOld && BitBlt(hdcMem,0,0,bw,bh,hScreen,_x0,_y0,SRCCOPY) && SelectObject(hdcMem,hOld)) {
                BITMAPINFOHEADER bmi;
                bmi.biSize = sizeof(BITMAPINFOHEADER);
                bmi.biWidth = bw;
                bmi.biHeight = -bh;
                bmi.biPlanes = 1;
                bmi.biBitCount = 32;
                bmi.biCompression = BI_RGB;
                bmi.biSizeImage = 0;
                bmi.biXPelsPerMeter = bmi.biYPelsPerMeter = 0;
                bmi.biClrUsed = bmi.biClrImportant = 0;
                unsigned char *buf = new unsigned char[4*bw*bh];
                if (GetDIBits(hdcMem,hBitmap,0,bh,buf,(BITMAPINFO*)&bmi,DIB_RGB_COLORS)) {
                  img.assign(bw,bh,1,3);
                  const unsigned char *ptrs = buf;
                  T *pR = img.data(0,0,0,0), *pG = img.data(0,0,0,1), *pB = img.data(0,0,0,2);
                  cimg_forXY(img,x,y) {
                    *(pR++) = (T)ptrs[2];
                    *(pG++) = (T)ptrs[1];
                    *(pB++) = (T)ptrs[0];
                    ptrs+=4;
                  }
                }
                delete[] buf;
              }
              DeleteObject(hBitmap);
            }
            DeleteDC(hdcMem);
          }
        }
        ReleaseDC(GetDesktopWindow(),hScreen);
      }
      if (img.is_empty())
        throw CImgDisplayException("CImgDisplay::screenshot(): Failed to take screenshot "
                                   "with coordinates (%d,%d)-(%d,%d).",
                                   x0,y0,x1,y1);