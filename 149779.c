static int IsAlpha(int enc, int c)
{
  if (enc == ASCII)
    return isalpha(c);

  if (c >= 0x41 && c <= 0x5a) return 1;
  if (c >= 0x61 && c <= 0x7a) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
  case ISO_8859_1:
  case ISO_8859_9:
    if (c == 0xaa) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xff) return 1;
    break;

  case ISO_8859_2:
    if (c == 0xa1 || c == 0xa3) return 1;
    if (c == 0xa5 || c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c >= 0xae && c <= 0xaf) return 1;
    if (c == 0xb1 || c == 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbc) return 1;
    if (c >= 0xbe && c <= 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_3:
    if (c == 0xa1) return 1;
    if (c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb1) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbc) return 1;
    if (c == 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xc2) return 1;
    if (c >= 0xc4 && c <= 0xcf) return 1;
    if (c >= 0xd1 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xe2) return 1;
    if (c >= 0xe4 && c <= 0xef) return 1;
    if (c >= 0xf1 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_4:
    if (c >= 0xa1 && c <= 0xa3) return 1;
    if (c == 0xa5 || c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c == 0xae) return 1;
    if (c == 0xb1 || c == 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_5:
    if (c >= 0xa1 && c <= 0xcf && c != 0xad) return 1;
    if (c >= 0xd0 && c <= 0xff && c != 0xf0 && c != 0xfd) return 1;
    break;

  case ISO_8859_6:
    if (c >= 0xc1 && c <= 0xda) return 1;
    if (c >= 0xe0 && c <= 0xf2) return 1;
    break;

  case ISO_8859_7:
    if (c == 0xb6) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c == 0xbc) return 1;
    if (c >= 0xbe && c <= 0xbf) return 1;
    if (c == 0xc0) return 1;
    if (c >= 0xc1 && c <= 0xdb && c != 0xd2) return 1;
    if (c >= 0xdc && c <= 0xfe) return 1;
    break;

  case ISO_8859_8:
    if (c == 0xb5) return 1;
    if (c >= 0xe0 && c <= 0xfa) return 1;
    break;

  case ISO_8859_10:
    if (c >= 0xa1 && c <= 0xa6) return 1;
    if (c >= 0xa8 && c <= 0xac) return 1;
    if (c == 0xae || c == 0xaf) return 1;
    if (c >= 0xb1 && c <= 0xb6) return 1;
    if (c >= 0xb8 && c <= 0xbc) return 1;
    if (c >= 0xbe && c <= 0xff) return 1;
    break;

  case ISO_8859_11:
    if (c >= 0xa1 && c <= 0xda) return 1;
    if (c >= 0xdf && c <= 0xfb) return 1;
    break;

  case ISO_8859_13:
    if (c == 0xa8) return 1;
    if (c == 0xaa) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xb8) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xbf && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_14:
    if (c == 0xa1 || c == 0xa2) return 1;
    if (c == 0xa4 || c == 0xa5) return 1;
    if (c == 0xa6 || c == 0xa8) return 1;
    if (c >= 0xaa && c <= 0xac) return 1;
    if (c >= 0xaf && c <= 0xb5) return 1;
    if (c >= 0xb7 && c <= 0xff) return 1;
    break;

  case ISO_8859_15:
    if (c == 0xaa) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xff) return 1;
    if (c == 0xa6) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xb4) return 1;
    if (c == 0xb8) return 1;
    if (c == 0xbc) return 1;
    if (c == 0xbd) return 1;
    if (c == 0xbe) return 1;
    break;

  case ISO_8859_16:
    if (c == 0xa1) return 1;
    if (c == 0xa2) return 1;
    if (c == 0xa3) return 1;
    if (c == 0xa6) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xaa) return 1;
    if (c == 0xac) return 1;
    if (c == 0xae) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb2) return 1;
    if (c == 0xb3) return 1;
    if (c == 0xb4) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c == 0xbc) return 1;
    if (c == 0xbd) return 1;
    if (c == 0xbe) return 1;
    if (c == 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xde) return 1;
    if (c >= 0xdf && c <= 0xff) return 1;
    break;

  case KOI8_R:
    if (c == 0xa3 || c == 0xb3) return 1;
    /* fall */
  case KOI8:
    if (c >= 0xc0 && c <= 0xff) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}