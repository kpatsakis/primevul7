    **/
    static unsigned int keycode(const char *const keycode) {
#define _cimg_keycode(k) if (!cimg::strcasecmp(keycode,#k)) return cimg::key##k;
      _cimg_keycode(ESC); _cimg_keycode(F1); _cimg_keycode(F2); _cimg_keycode(F3);
      _cimg_keycode(F4); _cimg_keycode(F5); _cimg_keycode(F6); _cimg_keycode(F7);
      _cimg_keycode(F8); _cimg_keycode(F9); _cimg_keycode(F10); _cimg_keycode(F11);
      _cimg_keycode(F12); _cimg_keycode(PAUSE); _cimg_keycode(1); _cimg_keycode(2);
      _cimg_keycode(3); _cimg_keycode(4); _cimg_keycode(5); _cimg_keycode(6);
      _cimg_keycode(7); _cimg_keycode(8); _cimg_keycode(9); _cimg_keycode(0);
      _cimg_keycode(BACKSPACE); _cimg_keycode(INSERT); _cimg_keycode(HOME);
      _cimg_keycode(PAGEUP); _cimg_keycode(TAB); _cimg_keycode(Q); _cimg_keycode(W);
      _cimg_keycode(E); _cimg_keycode(R); _cimg_keycode(T); _cimg_keycode(Y);
      _cimg_keycode(U); _cimg_keycode(I); _cimg_keycode(O); _cimg_keycode(P);
      _cimg_keycode(DELETE); _cimg_keycode(END); _cimg_keycode(PAGEDOWN);
      _cimg_keycode(CAPSLOCK); _cimg_keycode(A); _cimg_keycode(S); _cimg_keycode(D);
      _cimg_keycode(F); _cimg_keycode(G); _cimg_keycode(H); _cimg_keycode(J);
      _cimg_keycode(K); _cimg_keycode(L); _cimg_keycode(ENTER);
      _cimg_keycode(SHIFTLEFT); _cimg_keycode(Z); _cimg_keycode(X); _cimg_keycode(C);
      _cimg_keycode(V); _cimg_keycode(B); _cimg_keycode(N); _cimg_keycode(M);
      _cimg_keycode(SHIFTRIGHT); _cimg_keycode(ARROWUP); _cimg_keycode(CTRLLEFT);
      _cimg_keycode(APPLEFT); _cimg_keycode(ALT); _cimg_keycode(SPACE); _cimg_keycode(ALTGR);
      _cimg_keycode(APPRIGHT); _cimg_keycode(MENU); _cimg_keycode(CTRLRIGHT);
      _cimg_keycode(ARROWLEFT); _cimg_keycode(ARROWDOWN); _cimg_keycode(ARROWRIGHT);
      _cimg_keycode(PAD0); _cimg_keycode(PAD1); _cimg_keycode(PAD2);
      _cimg_keycode(PAD3); _cimg_keycode(PAD4); _cimg_keycode(PAD5);
      _cimg_keycode(PAD6); _cimg_keycode(PAD7); _cimg_keycode(PAD8);
      _cimg_keycode(PAD9); _cimg_keycode(PADADD); _cimg_keycode(PADSUB);
      _cimg_keycode(PADMUL); _cimg_keycode(PADDIV);
      return 0;