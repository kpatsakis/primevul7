expand_line(word *line, int linesize, int bpp, int ebpp)
{
  int endline = linesize;
  byte *start = (byte *)line;
  register byte *in, *out;

  if (bpp == 16)              /* 16 to 24 (cmy) if required */
    { register byte b0, b1;
      endline = ((endline + 1) / 2);
      in = start + endline * 2;
      out = start + (endline *= 3);

      while (in > start)
        { b0 = *--in;
          b1 = *--in;
          *--out = (b0 << 3) + ((b0 >> 2) & 0x7);
          *--out = (b1 << 5) + ((b0 >> 3)  & 0x1c) + ((b1 >> 1) & 0x3);
          *--out = (b1 & 0xf8) + (b1 >> 5);
        }
    }

  if (ebpp == 32)             /* 24 (cmy) to 32 (cmyk) if required */
    { register byte c, m, y, k;
      endline = ((endline + 2) / 3);
      in = start + endline * 3;
      out = start + endline * 4;

      while (in > start)
        { y = *--in;
          m = *--in;
          c = *--in;
          k = c < m ? (c < y ? c : y) : (m < y ? m : y);
          *--out = y - k;
          *--out = m - k;
          *--out = c - k;
          *--out = k;
        }
    }
}