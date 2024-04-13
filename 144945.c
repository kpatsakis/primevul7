    **/
    bool is_key(const unsigned int keycode) const {
#define _cimg_iskey_test(k) if (keycode==cimg::key##k) return _is_key##k;
      _cimg_iskey_test(ESC); _cimg_iskey_test(F1); _cimg_iskey_test(F2); _cimg_iskey_test(F3);
      _cimg_iskey_test(F4); _cimg_iskey_test(F5); _cimg_iskey_test(F6); _cimg_iskey_test(F7);
      _cimg_iskey_test(F8); _cimg_iskey_test(F9); _cimg_iskey_test(F10); _cimg_iskey_test(F11);
      _cimg_iskey_test(F12); _cimg_iskey_test(PAUSE); _cimg_iskey_test(1); _cimg_iskey_test(2);
      _cimg_iskey_test(3); _cimg_iskey_test(4); _cimg_iskey_test(5); _cimg_iskey_test(6);
      _cimg_iskey_test(7); _cimg_iskey_test(8); _cimg_iskey_test(9); _cimg_iskey_test(0);
      _cimg_iskey_test(BACKSPACE); _cimg_iskey_test(INSERT); _cimg_iskey_test(HOME);
      _cimg_iskey_test(PAGEUP); _cimg_iskey_test(TAB); _cimg_iskey_test(Q); _cimg_iskey_test(W);
      _cimg_iskey_test(E); _cimg_iskey_test(R); _cimg_iskey_test(T); _cimg_iskey_test(Y);
      _cimg_iskey_test(U); _cimg_iskey_test(I); _cimg_iskey_test(O); _cimg_iskey_test(P);
      _cimg_iskey_test(DELETE); _cimg_iskey_test(END); _cimg_iskey_test(PAGEDOWN);
      _cimg_iskey_test(CAPSLOCK); _cimg_iskey_test(A); _cimg_iskey_test(S); _cimg_iskey_test(D);
      _cimg_iskey_test(F); _cimg_iskey_test(G); _cimg_iskey_test(H); _cimg_iskey_test(J);
      _cimg_iskey_test(K); _cimg_iskey_test(L); _cimg_iskey_test(ENTER);
      _cimg_iskey_test(SHIFTLEFT); _cimg_iskey_test(Z); _cimg_iskey_test(X); _cimg_iskey_test(C);
      _cimg_iskey_test(V); _cimg_iskey_test(B); _cimg_iskey_test(N); _cimg_iskey_test(M);
      _cimg_iskey_test(SHIFTRIGHT); _cimg_iskey_test(ARROWUP); _cimg_iskey_test(CTRLLEFT);
      _cimg_iskey_test(APPLEFT); _cimg_iskey_test(ALT); _cimg_iskey_test(SPACE); _cimg_iskey_test(ALTGR);
      _cimg_iskey_test(APPRIGHT); _cimg_iskey_test(MENU); _cimg_iskey_test(CTRLRIGHT);
      _cimg_iskey_test(ARROWLEFT); _cimg_iskey_test(ARROWDOWN); _cimg_iskey_test(ARROWRIGHT);
      _cimg_iskey_test(PAD0); _cimg_iskey_test(PAD1); _cimg_iskey_test(PAD2);
      _cimg_iskey_test(PAD3); _cimg_iskey_test(PAD4); _cimg_iskey_test(PAD5);
      _cimg_iskey_test(PAD6); _cimg_iskey_test(PAD7); _cimg_iskey_test(PAD8);
      _cimg_iskey_test(PAD9); _cimg_iskey_test(PADADD); _cimg_iskey_test(PADSUB);
      _cimg_iskey_test(PADMUL); _cimg_iskey_test(PADDIV);
      return false;