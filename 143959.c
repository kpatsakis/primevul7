
    CImgDisplay& paint() {
      if (_is_closed) return *this;
      WaitForSingleObject(_mutex,INFINITE);
      SetDIBitsToDevice(_hdc,0,0,_width,_height,0,0,0,_height,_data,&_bmi,DIB_RGB_COLORS);
      ReleaseMutex(_mutex);
      return *this;