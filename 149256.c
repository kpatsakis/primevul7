add_ctype_to_cc_by_range_limit(CClassNode* cc, int ctype ARG_UNUSED, int not,
                               OnigEncoding enc ARG_UNUSED,
                               OnigCodePoint sb_out,
                               const OnigCodePoint mbr[], OnigCodePoint limit)
{
  int i, r;
  OnigCodePoint j;
  OnigCodePoint from;
  OnigCodePoint to;

  int n = ONIGENC_CODE_RANGE_NUM(mbr);

  if (not == 0) {
    for (i = 0; i < n; i++) {
      for (j  = ONIGENC_CODE_RANGE_FROM(mbr, i);
           j <= ONIGENC_CODE_RANGE_TO(mbr, i); j++) {
        if (j > limit) goto end;
        if (j >= sb_out) {
          if (j > ONIGENC_CODE_RANGE_FROM(mbr, i)) {
            to = ONIGENC_CODE_RANGE_TO(mbr, i);
            if (to > limit) to = limit;
            r = add_code_range_to_buf(&(cc->mbuf), j, to);
            if (r != 0) return r;
            i++;
          }

          goto sb_end;
        }
        BITSET_SET_BIT(cc->bs, j);
      }
    }

  sb_end:
    for ( ; i < n; i++) {
      from = ONIGENC_CODE_RANGE_FROM(mbr, i);
      to   = ONIGENC_CODE_RANGE_TO(mbr, i);
      if (from > limit) break;
      if (to   > limit) to = limit;
      r = add_code_range_to_buf(&(cc->mbuf), from, to);
      if (r != 0) return r;
    }
  }
  else {
    OnigCodePoint prev = 0;

    for (i = 0; i < n; i++) {
      from = ONIGENC_CODE_RANGE_FROM(mbr, i);
      if (from > limit) {
        for (j = prev; j < sb_out; j++) {
          BITSET_SET_BIT(cc->bs, j);
        }
        goto sb_end2;
      }
      for (j = prev; j < from; j++) {
        if (j >= sb_out) goto sb_end2;
        BITSET_SET_BIT(cc->bs, j);
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i);
      if (prev > limit) prev = limit;
      prev++;
      if (prev == 0) goto end;
    }
    for (j = prev; j < sb_out; j++) {
      BITSET_SET_BIT(cc->bs, j);
    }

  sb_end2:
    prev = sb_out;

    for (i = 0; i < n; i++) {
      from = ONIGENC_CODE_RANGE_FROM(mbr, i);
      if (from > limit) goto last;

      if (prev < from) {
        r = add_code_range_to_buf(&(cc->mbuf), prev, from - 1);
        if (r != 0) return r;
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i);
      if (prev > limit) prev = limit;
      prev++;
      if (prev == 0) goto end;
    }

  last:
    r = add_code_range_to_buf(&(cc->mbuf), prev, MAX_CODE_POINT);
    if (r != 0) return r;
  }

 end:
  return 0;
}