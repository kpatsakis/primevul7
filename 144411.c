    **/
    CImgDisplay& set_key(const unsigned int keycode, const bool is_pressed=true) {
#define _cimg_set_key(k) if (keycode==cimg::key##k) _is_key##k = is_pressed;
      _cimg_set_key(ESC); _cimg_set_key(F1); _cimg_set_key(F2); _cimg_set_key(F3);
      _cimg_set_key(F4); _cimg_set_key(F5); _cimg_set_key(F6); _cimg_set_key(F7);
      _cimg_set_key(F8); _cimg_set_key(F9); _cimg_set_key(F10); _cimg_set_key(F11);
      _cimg_set_key(F12); _cimg_set_key(PAUSE); _cimg_set_key(1); _cimg_set_key(2);
      _cimg_set_key(3); _cimg_set_key(4); _cimg_set_key(5); _cimg_set_key(6);
      _cimg_set_key(7); _cimg_set_key(8); _cimg_set_key(9); _cimg_set_key(0);
      _cimg_set_key(BACKSPACE); _cimg_set_key(INSERT); _cimg_set_key(HOME);
      _cimg_set_key(PAGEUP); _cimg_set_key(TAB); _cimg_set_key(Q); _cimg_set_key(W);
      _cimg_set_key(E); _cimg_set_key(R); _cimg_set_key(T); _cimg_set_key(Y);
      _cimg_set_key(U); _cimg_set_key(I); _cimg_set_key(O); _cimg_set_key(P);
      _cimg_set_key(DELETE); _cimg_set_key(END); _cimg_set_key(PAGEDOWN);
      _cimg_set_key(CAPSLOCK); _cimg_set_key(A); _cimg_set_key(S); _cimg_set_key(D);
      _cimg_set_key(F); _cimg_set_key(G); _cimg_set_key(H); _cimg_set_key(J);
      _cimg_set_key(K); _cimg_set_key(L); _cimg_set_key(ENTER);
      _cimg_set_key(SHIFTLEFT); _cimg_set_key(Z); _cimg_set_key(X); _cimg_set_key(C);
      _cimg_set_key(V); _cimg_set_key(B); _cimg_set_key(N); _cimg_set_key(M);
      _cimg_set_key(SHIFTRIGHT); _cimg_set_key(ARROWUP); _cimg_set_key(CTRLLEFT);
      _cimg_set_key(APPLEFT); _cimg_set_key(ALT); _cimg_set_key(SPACE); _cimg_set_key(ALTGR);
      _cimg_set_key(APPRIGHT); _cimg_set_key(MENU); _cimg_set_key(CTRLRIGHT);
      _cimg_set_key(ARROWLEFT); _cimg_set_key(ARROWDOWN); _cimg_set_key(ARROWRIGHT);
      _cimg_set_key(PAD0); _cimg_set_key(PAD1); _cimg_set_key(PAD2);
      _cimg_set_key(PAD3); _cimg_set_key(PAD4); _cimg_set_key(PAD5);
      _cimg_set_key(PAD6); _cimg_set_key(PAD7); _cimg_set_key(PAD8);
      _cimg_set_key(PAD9); _cimg_set_key(PADADD); _cimg_set_key(PADSUB);
      _cimg_set_key(PADMUL); _cimg_set_key(PADDIV);
      if (is_pressed) {
        if (*_keys)
          std::memmove((void*)(_keys + 1),(void*)_keys,127*sizeof(unsigned int));
        *_keys = keycode;
        if (*_released_keys) {
          std::memmove((void*)(_released_keys + 1),(void*)_released_keys,127*sizeof(unsigned int));
          *_released_keys = 0;
        }
      } else {
        if (*_keys) {
          std::memmove((void*)(_keys + 1),(void*)_keys,127*sizeof(unsigned int));
          *_keys = 0;
        }
        if (*_released_keys)
          std::memmove((void*)(_released_keys + 1),(void*)_released_keys,127*sizeof(unsigned int));
        *_released_keys = keycode;
      }
      _is_event = keycode?true:false;
      if (keycode) {
#if cimg_display==1
        pthread_cond_broadcast(&cimg::X11_attr().wait_event);
#elif cimg_display==2
        SetEvent(cimg::Win32_attr().wait_event);
#endif
      }
      return *this;