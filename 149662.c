read_escaped_byte(const char **pp, const char *end, onig_errmsg_buffer err)
{
    const char *p = *pp;
    int code;
    int meta_prefix = 0, ctrl_prefix = 0;
    size_t len;

    if (p == end || *p++ != '\\') {
        errcpy(err, "too short escaped multibyte character");
        return -1;
    }

again:
    if (p == end) {
        errcpy(err, "too short escape sequence");
        return -1;
    }
    switch (*p++) {
      case '\\': code = '\\'; break;
      case 'n': code = '\n'; break;
      case 't': code = '\t'; break;
      case 'r': code = '\r'; break;
      case 'f': code = '\f'; break;
      case 'v': code = '\013'; break;
      case 'a': code = '\007'; break;
      case 'e': code = '\033'; break;

      /* \OOO */
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
        p--;
        code = scan_oct(p, end < p+3 ? end-p : 3, &len);
        p += len;
        break;

      case 'x': /* \xHH */
        code = scan_hex(p, end < p+2 ? end-p : 2, &len);
        if (len < 1) {
            errcpy(err, "invalid hex escape");
            return -1;
        }
        p += len;
        break;

      case 'M': /* \M-X, \M-\C-X, \M-\cX */
        if (meta_prefix) {
            errcpy(err, "duplicate meta escape");
            return -1;
        }
        meta_prefix = 1;
        if (p+1 < end && *p++ == '-' && (*p & 0x80) == 0) {
            if (*p == '\\') {
                p++;
                goto again;
            }
            else {
                code = *p++;
                break;
            }
        }
        errcpy(err, "too short meta escape");
        return -1;

      case 'C': /* \C-X, \C-\M-X */
        if (p == end || *p++ != '-') {
            errcpy(err, "too short control escape");
            return -1;
        }
      case 'c': /* \cX, \c\M-X */
        if (ctrl_prefix) {
            errcpy(err, "duplicate control escape");
            return -1;
        }
        ctrl_prefix = 1;
        if (p < end && (*p & 0x80) == 0) {
            if (*p == '\\') {
                p++;
                goto again;
            }
            else {
                code = *p++;
                break;
            }
        }
        errcpy(err, "too short control escape");
        return -1;

      default:
        errcpy(err, "unexpected escape sequence");
        return -1;
    }
    if (code < 0 || 0xff < code) {
        errcpy(err, "invalid escape code");
        return -1;
    }

    if (ctrl_prefix)
        code &= 0x1f;
    if (meta_prefix)
        code |= 0x80;

    *pp = p;
    return code;
}