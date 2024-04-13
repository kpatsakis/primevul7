match_at(regex_t* reg, const UChar* str, const UChar* end,
#ifdef USE_MATCH_RANGE_MUST_BE_INSIDE_OF_SPECIFIED_RANGE
	 const UChar* right_range,
#endif
	 const UChar* sstart, UChar* sprev, OnigMatchArg* msa)
{
  static const UChar FinishCode[] = { OP_FINISH };

  int i, num_mem, pop_level;
  ptrdiff_t n, best_len;
  LengthType tlen, tlen2;
  MemNumType mem;
  RelAddrType addr;
  OnigOptionType option = reg->options;
  OnigEncoding encode = reg->enc;
  OnigCaseFoldType case_fold_flag = reg->case_fold_flag;
  UChar *s, *q, *sbegin;
  UChar *p = reg->p;
  UChar *pkeep;
  char *alloca_base;
  char *xmalloc_base = NULL;
  OnigStackType *stk_alloc, *stk_base, *stk, *stk_end;
  OnigStackType *stkp; /* used as any purpose. */
  OnigStackIndex si;
  OnigStackIndex *repeat_stk;
  OnigStackIndex *mem_start_stk, *mem_end_stk;
#ifdef USE_COMBINATION_EXPLOSION_CHECK
  int scv;
  unsigned char* state_check_buff = msa->state_check_buff;
  int num_comb_exp_check = reg->num_comb_exp_check;
#endif

#if USE_TOKEN_THREADED_VM
# define OP_OFFSET  1
# define VM_LOOP JUMP;
# define VM_LOOP_END
# define CASE(x) L_##x: sbegin = s; OPCODE_EXEC_HOOK;
# define DEFAULT L_DEFAULT:
# define NEXT sprev = sbegin; JUMP
# define JUMP goto *oplabels[*p++]

  static const void *oplabels[] = {
    &&L_OP_FINISH,               /* matching process terminator (no more alternative) */
    &&L_OP_END,                  /* pattern code terminator (success end) */

    &&L_OP_EXACT1,               /* single byte, N = 1 */
    &&L_OP_EXACT2,               /* single byte, N = 2 */
    &&L_OP_EXACT3,               /* single byte, N = 3 */
    &&L_OP_EXACT4,               /* single byte, N = 4 */
    &&L_OP_EXACT5,               /* single byte, N = 5 */
    &&L_OP_EXACTN,               /* single byte */
    &&L_OP_EXACTMB2N1,           /* mb-length = 2 N = 1 */
    &&L_OP_EXACTMB2N2,           /* mb-length = 2 N = 2 */
    &&L_OP_EXACTMB2N3,           /* mb-length = 2 N = 3 */
    &&L_OP_EXACTMB2N,            /* mb-length = 2 */
    &&L_OP_EXACTMB3N,            /* mb-length = 3 */
    &&L_OP_EXACTMBN,             /* other length */

    &&L_OP_EXACT1_IC,            /* single byte, N = 1, ignore case */
    &&L_OP_EXACTN_IC,            /* single byte,        ignore case */

    &&L_OP_CCLASS,
    &&L_OP_CCLASS_MB,
    &&L_OP_CCLASS_MIX,
    &&L_OP_CCLASS_NOT,
    &&L_OP_CCLASS_MB_NOT,
    &&L_OP_CCLASS_MIX_NOT,

    &&L_OP_ANYCHAR,                 /* "."  */
    &&L_OP_ANYCHAR_ML,              /* "."  multi-line */
    &&L_OP_ANYCHAR_STAR,            /* ".*" */
    &&L_OP_ANYCHAR_ML_STAR,         /* ".*" multi-line */
    &&L_OP_ANYCHAR_STAR_PEEK_NEXT,
    &&L_OP_ANYCHAR_ML_STAR_PEEK_NEXT,

    &&L_OP_WORD,
    &&L_OP_NOT_WORD,
    &&L_OP_WORD_BOUND,
    &&L_OP_NOT_WORD_BOUND,
# ifdef USE_WORD_BEGIN_END
    &&L_OP_WORD_BEGIN,
    &&L_OP_WORD_END,
# else
    &&L_DEFAULT,
    &&L_DEFAULT,
# endif
    &&L_OP_ASCII_WORD,
    &&L_OP_NOT_ASCII_WORD,
    &&L_OP_ASCII_WORD_BOUND,
    &&L_OP_NOT_ASCII_WORD_BOUND,
# ifdef USE_WORD_BEGIN_END
    &&L_OP_ASCII_WORD_BEGIN,
    &&L_OP_ASCII_WORD_END,
# else
    &&L_DEFAULT,
    &&L_DEFAULT,
# endif

    &&L_OP_BEGIN_BUF,
    &&L_OP_END_BUF,
    &&L_OP_BEGIN_LINE,
    &&L_OP_END_LINE,
    &&L_OP_SEMI_END_BUF,
    &&L_OP_BEGIN_POSITION,
    &&L_OP_BEGIN_POS_OR_LINE,    /* used for implicit anchor optimization */

    &&L_OP_BACKREF1,
    &&L_OP_BACKREF2,
    &&L_OP_BACKREFN,
    &&L_OP_BACKREFN_IC,
    &&L_OP_BACKREF_MULTI,
    &&L_OP_BACKREF_MULTI_IC,
# ifdef USE_BACKREF_WITH_LEVEL
    &&L_OP_BACKREF_WITH_LEVEL,   /* \k<xxx+n>, \k<xxx-n> */
# else
    &&L_DEFAULT,
# endif
    &&L_OP_MEMORY_START,
    &&L_OP_MEMORY_START_PUSH,    /* push back-tracker to stack */
    &&L_OP_MEMORY_END_PUSH,      /* push back-tracker to stack */
# ifdef USE_SUBEXP_CALL
    &&L_OP_MEMORY_END_PUSH_REC,  /* push back-tracker to stack */
# else
    &&L_DEFAULT,
# endif
    &&L_OP_MEMORY_END,
# ifdef USE_SUBEXP_CALL
    &&L_OP_MEMORY_END_REC,       /* push marker to stack */
# else
    &&L_DEFAULT,
# endif

    &&L_OP_KEEP,

    &&L_OP_FAIL,                 /* pop stack and move */
    &&L_OP_JUMP,
    &&L_OP_PUSH,
    &&L_OP_POP,
# ifdef USE_OP_PUSH_OR_JUMP_EXACT
    &&L_OP_PUSH_OR_JUMP_EXACT1,  /* if match exact then push, else jump. */
# else
    &&L_DEFAULT,
# endif
    &&L_OP_PUSH_IF_PEEK_NEXT,    /* if match exact then push, else none. */
    &&L_OP_REPEAT,               /* {n,m} */
    &&L_OP_REPEAT_NG,            /* {n,m}? (non greedy) */
    &&L_OP_REPEAT_INC,
    &&L_OP_REPEAT_INC_NG,        /* non greedy */
    &&L_OP_REPEAT_INC_SG,        /* search and get in stack */
    &&L_OP_REPEAT_INC_NG_SG,     /* search and get in stack (non greedy) */
    &&L_OP_NULL_CHECK_START,     /* null loop checker start */
    &&L_OP_NULL_CHECK_END,       /* null loop checker end   */
# ifdef USE_MONOMANIAC_CHECK_CAPTURES_IN_ENDLESS_REPEAT
    &&L_OP_NULL_CHECK_END_MEMST, /* null loop checker end (with capture status) */
# else
    &&L_DEFAULT,
# endif
# ifdef USE_SUBEXP_CALL
    &&L_OP_NULL_CHECK_END_MEMST_PUSH, /* with capture status and push check-end */
# else
    &&L_DEFAULT,
# endif

    &&L_OP_PUSH_POS,             /* (?=...)  start */
    &&L_OP_POP_POS,              /* (?=...)  end   */
    &&L_OP_PUSH_POS_NOT,         /* (?!...)  start */
    &&L_OP_FAIL_POS,             /* (?!...)  end   */
    &&L_OP_PUSH_STOP_BT,         /* (?>...)  start */
    &&L_OP_POP_STOP_BT,          /* (?>...)  end   */
    &&L_OP_LOOK_BEHIND,          /* (?<=...) start (no needs end opcode) */
    &&L_OP_PUSH_LOOK_BEHIND_NOT, /* (?<!...) start */
    &&L_OP_FAIL_LOOK_BEHIND_NOT, /* (?<!...) end   */

# ifdef USE_SUBEXP_CALL
    &&L_OP_CALL,                 /* \g<name> */
    &&L_OP_RETURN,
# else
    &&L_DEFAULT,
    &&L_DEFAULT,
# endif
    &&L_OP_CONDITION,

# ifdef USE_COMBINATION_EXPLOSION_CHECK
    &&L_OP_STATE_CHECK_PUSH,         /* combination explosion check and push */
    &&L_OP_STATE_CHECK_PUSH_OR_JUMP, /* check ok -> push, else jump  */
    &&L_OP_STATE_CHECK,              /* check only */
# else
    &&L_DEFAULT,
    &&L_DEFAULT,
    &&L_DEFAULT,
# endif
# ifdef USE_COMBINATION_EXPLOSION_CHECK
    &&L_OP_STATE_CHECK_ANYCHAR_STAR,
    &&L_OP_STATE_CHECK_ANYCHAR_ML_STAR,
# else
    &&L_DEFAULT,
    &&L_DEFAULT,
# endif
    /* no need: IS_DYNAMIC_OPTION() == 0 */
# if 0   /* no need: IS_DYNAMIC_OPTION() == 0 */
    &&L_OP_SET_OPTION_PUSH,    /* set option and push recover option */
    &&L_OP_SET_OPTION          /* set option */
# else
    &&L_DEFAULT,
    &&L_DEFAULT
# endif
  };
#else /* USE_TOKEN_THREADED_VM */

# define OP_OFFSET  0
# define VM_LOOP                                \
  while (1) {                                   \
  OPCODE_EXEC_HOOK;                             \
  sbegin = s;                                   \
  switch (*p++) {
# define VM_LOOP_END } sprev = sbegin; }
# define CASE(x) case x:
# define DEFAULT default:
# define NEXT break
# define JUMP continue; break
#endif /* USE_TOKEN_THREADED_VM */


#ifdef USE_SUBEXP_CALL
/* Stack #0 is used to store the pattern itself and used for (?R), \g<0>,
   etc. Additional space is required. */
# define ADD_NUMMEM 1
#else
/* Stack #0 not is used. */
# define ADD_NUMMEM 0
#endif

  n = reg->num_repeat + (reg->num_mem + ADD_NUMMEM) * 2;

  STACK_INIT(alloca_base, xmalloc_base, n, INIT_MATCH_STACK_SIZE);
  pop_level = reg->stack_pop_level;
  num_mem = reg->num_mem;
  repeat_stk = (OnigStackIndex* )alloca_base;

  mem_start_stk = (OnigStackIndex* )(repeat_stk + reg->num_repeat);
  mem_end_stk   = mem_start_stk + (num_mem + ADD_NUMMEM);
  {
    OnigStackIndex *pp = mem_start_stk;
    for (; pp < repeat_stk + n; pp += 2) {
      pp[0] = INVALID_STACK_INDEX;
      pp[1] = INVALID_STACK_INDEX;
    }
  }
#ifndef USE_SUBEXP_CALL
  mem_start_stk--; /* for index start from 1,
		      mem_start_stk[1]..mem_start_stk[num_mem] */
  mem_end_stk--;   /* for index start from 1,
		      mem_end_stk[1]..mem_end_stk[num_mem] */
#endif

#ifdef ONIG_DEBUG_MATCH
  fprintf(stderr, "match_at: str: %"PRIdPTR" (%p), end: %"PRIdPTR" (%p), start: %"PRIdPTR" (%p), sprev: %"PRIdPTR" (%p)\n",
	  (intptr_t )str, str, (intptr_t )end, end, (intptr_t )sstart, sstart, (intptr_t )sprev, sprev);
  fprintf(stderr, "size: %d, start offset: %d\n",
	  (int )(end - str), (int )(sstart - str));
  fprintf(stderr, "\n ofs> str                   stk:type   addr:opcode\n");
#endif

  STACK_PUSH_ENSURED(STK_ALT, (UChar* )FinishCode);  /* bottom stack */
  best_len = ONIG_MISMATCH;
  s = (UChar* )sstart;
  pkeep = (UChar* )sstart;


#ifdef ONIG_DEBUG_MATCH
# define OPCODE_EXEC_HOOK                                               \
    if (s) {                                                            \
      UChar *op, *q, *bp, buf[50];                                      \
      int len;                                                          \
      op = p - OP_OFFSET;                                               \
      fprintf(stderr, "%4"PRIdPTR"> \"", (*op == OP_FINISH) ? (ptrdiff_t )-1 : s - str); \
      bp = buf;                                                         \
      q = s;                                                            \
      if (*op != OP_FINISH) {    /* s may not be a valid pointer if OP_FINISH. */ \
	for (i = 0; i < 7 && q < end; i++) {                            \
	  len = enclen(encode, q, end);                                 \
	  while (len-- > 0) *bp++ = *q++;                               \
	}                                                               \
        if (q < end) { xmemcpy(bp, "...", 3); bp += 3; }                \
      }                                                                 \
      xmemcpy(bp, "\"", 1); bp += 1;                                    \
      *bp = 0;                                                          \
      fputs((char* )buf, stderr);                                       \
      for (i = 0; i < 20 - (bp - buf); i++) fputc(' ', stderr);         \
      fprintf(stderr, "%4"PRIdPTR":%s %4"PRIdPTR":",                    \
	  stk - stk_base - 1,                                           \
	  (stk > stk_base) ? stack_type_str(stk[-1].type) : "      ",   \
	  (op == FinishCode) ? (ptrdiff_t )-1 : op - reg->p);           \
      onig_print_compiled_byte_code(stderr, op, reg->p+reg->used, NULL, encode); \
      fprintf(stderr, "\n");                                            \
    }
#else
# define OPCODE_EXEC_HOOK ((void) 0)
#endif


  VM_LOOP {
    CASE(OP_END)  MOP_IN(OP_END);
      n = s - sstart;
      if (n > best_len) {
	OnigRegion* region;
#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
	if (IS_FIND_LONGEST(option)) {
	  if (n > msa->best_len) {
	    msa->best_len = n;
	    msa->best_s   = (UChar* )sstart;
	  }
	  else
	    goto end_best_len;
	}
#endif
	best_len = n;
	region = msa->region;
	if (region) {
	  region->beg[0] = ((pkeep > s) ? s : pkeep) - str;
	  region->end[0] = s - str;
	  for (i = 1; i <= num_mem; i++) {
	    if (mem_end_stk[i] != INVALID_STACK_INDEX) {
	      if (BIT_STATUS_AT(reg->bt_mem_start, i))
		region->beg[i] = STACK_AT(mem_start_stk[i])->u.mem.pstr - str;
	      else
		region->beg[i] = (UChar* )((void* )mem_start_stk[i]) - str;

	      region->end[i] = (BIT_STATUS_AT(reg->bt_mem_end, i)
				? STACK_AT(mem_end_stk[i])->u.mem.pstr
				: (UChar* )((void* )mem_end_stk[i])) - str;
	    }
	    else {
	      region->beg[i] = region->end[i] = ONIG_REGION_NOTPOS;
	    }
	  }

#ifdef USE_CAPTURE_HISTORY
	  if (reg->capture_history != 0) {
	    int r;
	    OnigCaptureTreeNode* node;

	    if (IS_NULL(region->history_root)) {
	      region->history_root = node = history_node_new();
	      CHECK_NULL_RETURN_MEMERR(node);
	    }
	    else {
	      node = region->history_root;
	      history_tree_clear(node);
	    }

	    node->group = 0;
	    node->beg   = ((pkeep > s) ? s : pkeep) - str;
	    node->end   = s - str;

	    stkp = stk_base;
	    r = make_capture_history_tree(region->history_root, &stkp,
		stk, (UChar* )str, reg);
	    if (r < 0) {
	      best_len = r; /* error code */
	      goto finish;
	    }
	  }
#endif /* USE_CAPTURE_HISTORY */
	} /* if (region) */
      } /* n > best_len */

#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
    end_best_len:
#endif
      MOP_OUT;

      if (IS_FIND_CONDITION(option)) {
	if (IS_FIND_NOT_EMPTY(option) && s == sstart) {
	  best_len = ONIG_MISMATCH;
	  goto fail; /* for retry */
	}
	if (IS_FIND_LONGEST(option) && DATA_ENSURE_CHECK1) {
	  goto fail; /* for retry */
	}
      }

      /* default behavior: return first-matching result. */
      goto finish;
      NEXT;

    CASE(OP_EXACT1)  MOP_IN(OP_EXACT1);
#if 0
      DATA_ENSURE(1);
      if (*p != *s) goto fail;
      p++; s++;
#endif
      if (*p != *s++) goto fail;
      DATA_ENSURE(0);
      p++;
      MOP_OUT;
      NEXT;

    CASE(OP_EXACT1_IC)  MOP_IN(OP_EXACT1_IC);
      {
	int len;
	UChar *q, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

	DATA_ENSURE(1);
	len = ONIGENC_MBC_CASE_FOLD(encode,
		    /* DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag), */
		    case_fold_flag,
		    &s, end, lowbuf);
	DATA_ENSURE(0);
	q = lowbuf;
	while (len-- > 0) {
	  if (*p != *q) {
	    goto fail;
	  }
	  p++; q++;
	}
      }
      MOP_OUT;
      NEXT;

    CASE(OP_EXACT2)  MOP_IN(OP_EXACT2);
      DATA_ENSURE(2);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      sprev = s;
      p++; s++;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACT3)  MOP_IN(OP_EXACT3);
      DATA_ENSURE(3);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      sprev = s;
      p++; s++;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACT4)  MOP_IN(OP_EXACT4);
      DATA_ENSURE(4);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      sprev = s;
      p++; s++;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACT5)  MOP_IN(OP_EXACT5);
      DATA_ENSURE(5);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      sprev = s;
      p++; s++;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACTN)  MOP_IN(OP_EXACTN);
      GET_LENGTH_INC(tlen, p);
      DATA_ENSURE(tlen);
      while (tlen-- > 0) {
	if (*p++ != *s++) goto fail;
      }
      sprev = s - 1;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACTN_IC)  MOP_IN(OP_EXACTN_IC);
      {
	int len;
	UChar *q, *endp, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

	GET_LENGTH_INC(tlen, p);
	endp = p + tlen;

	while (p < endp) {
	  sprev = s;
	  DATA_ENSURE(1);
	  len = ONIGENC_MBC_CASE_FOLD(encode,
		      /* DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag), */
		      case_fold_flag,
		      &s, end, lowbuf);
	  DATA_ENSURE(0);
	  q = lowbuf;
	  while (len-- > 0) {
	    if (*p != *q) goto fail;
	    p++; q++;
	  }
	}
      }

      MOP_OUT;
      JUMP;

    CASE(OP_EXACTMB2N1)  MOP_IN(OP_EXACTMB2N1);
      DATA_ENSURE(2);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      MOP_OUT;
      NEXT;

    CASE(OP_EXACTMB2N2)  MOP_IN(OP_EXACTMB2N2);
      DATA_ENSURE(4);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      sprev = s;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACTMB2N3)  MOP_IN(OP_EXACTMB2N3);
      DATA_ENSURE(6);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      sprev = s;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACTMB2N)  MOP_IN(OP_EXACTMB2N);
      GET_LENGTH_INC(tlen, p);
      DATA_ENSURE(tlen * 2);
      while (tlen-- > 0) {
	if (*p != *s) goto fail;
	p++; s++;
	if (*p != *s) goto fail;
	p++; s++;
      }
      sprev = s - 2;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACTMB3N)  MOP_IN(OP_EXACTMB3N);
      GET_LENGTH_INC(tlen, p);
      DATA_ENSURE(tlen * 3);
      while (tlen-- > 0) {
	if (*p != *s) goto fail;
	p++; s++;
	if (*p != *s) goto fail;
	p++; s++;
	if (*p != *s) goto fail;
	p++; s++;
      }
      sprev = s - 3;
      MOP_OUT;
      JUMP;

    CASE(OP_EXACTMBN)  MOP_IN(OP_EXACTMBN);
      GET_LENGTH_INC(tlen,  p);  /* mb-len */
      GET_LENGTH_INC(tlen2, p);  /* string len */
      tlen2 *= tlen;
      DATA_ENSURE(tlen2);
      while (tlen2-- > 0) {
	if (*p != *s) goto fail;
	p++; s++;
      }
      sprev = s - tlen;
      MOP_OUT;
      JUMP;

    CASE(OP_CCLASS)  MOP_IN(OP_CCLASS);
      DATA_ENSURE(1);
      if (BITSET_AT(((BitSetRef )p), *s) == 0) goto fail;
      p += SIZE_BITSET;
      s += enclen(encode, s, end);   /* OP_CCLASS can match mb-code. \D, \S */
      MOP_OUT;
      NEXT;

    CASE(OP_CCLASS_MB)  MOP_IN(OP_CCLASS_MB);
      if (! ONIGENC_IS_MBC_HEAD(encode, s, end)) goto fail;

    cclass_mb:
      GET_LENGTH_INC(tlen, p);
      {
	OnigCodePoint code;
	UChar *ss;
	int mb_len;

	DATA_ENSURE(1);
	mb_len = enclen(encode, s, end);
	DATA_ENSURE(mb_len);
	ss = s;
	s += mb_len;
	code = ONIGENC_MBC_TO_CODE(encode, ss, s);

#ifdef PLATFORM_UNALIGNED_WORD_ACCESS
	if (! onig_is_in_code_range(p, code)) goto fail;
#else
	q = p;
	ALIGNMENT_RIGHT(q);
	if (! onig_is_in_code_range(q, code)) goto fail;
#endif
      }
      p += tlen;
      MOP_OUT;
      NEXT;

    CASE(OP_CCLASS_MIX)  MOP_IN(OP_CCLASS_MIX);
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_HEAD(encode, s, end)) {
	p += SIZE_BITSET;
	goto cclass_mb;
      }
      else {
	if (BITSET_AT(((BitSetRef )p), *s) == 0)
	  goto fail;

	p += SIZE_BITSET;
	GET_LENGTH_INC(tlen, p);
	p += tlen;
	s++;
      }
      MOP_OUT;
      NEXT;

    CASE(OP_CCLASS_NOT)  MOP_IN(OP_CCLASS_NOT);
      DATA_ENSURE(1);
      if (BITSET_AT(((BitSetRef )p), *s) != 0) goto fail;
      p += SIZE_BITSET;
      s += enclen(encode, s, end);
      MOP_OUT;
      NEXT;

    CASE(OP_CCLASS_MB_NOT)  MOP_IN(OP_CCLASS_MB_NOT);
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_HEAD(encode, s, end)) {
	s++;
	GET_LENGTH_INC(tlen, p);
	p += tlen;
	goto cc_mb_not_success;
      }

    cclass_mb_not:
      GET_LENGTH_INC(tlen, p);
      {
	OnigCodePoint code;
	UChar *ss;
	int mb_len = enclen(encode, s, end);

	if (! DATA_ENSURE_CHECK(mb_len)) {
	  DATA_ENSURE(1);
	  s = (UChar* )end;
	  p += tlen;
	  goto cc_mb_not_success;
	}

	ss = s;
	s += mb_len;
	code = ONIGENC_MBC_TO_CODE(encode, ss, s);

#ifdef PLATFORM_UNALIGNED_WORD_ACCESS
	if (onig_is_in_code_range(p, code)) goto fail;
#else
	q = p;
	ALIGNMENT_RIGHT(q);
	if (onig_is_in_code_range(q, code)) goto fail;
#endif
      }
      p += tlen;

    cc_mb_not_success:
      MOP_OUT;
      NEXT;

    CASE(OP_CCLASS_MIX_NOT)  MOP_IN(OP_CCLASS_MIX_NOT);
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_HEAD(encode, s, end)) {
	p += SIZE_BITSET;
	goto cclass_mb_not;
      }
      else {
	if (BITSET_AT(((BitSetRef )p), *s) != 0)
	  goto fail;

	p += SIZE_BITSET;
	GET_LENGTH_INC(tlen, p);
	p += tlen;
	s++;
      }
      MOP_OUT;
      NEXT;

    CASE(OP_ANYCHAR)  MOP_IN(OP_ANYCHAR);
      DATA_ENSURE(1);
      n = enclen(encode, s, end);
      DATA_ENSURE(n);
      if (ONIGENC_IS_MBC_NEWLINE_EX(encode, s, str, end, option, 0)) goto fail;
      s += n;
      MOP_OUT;
      NEXT;

    CASE(OP_ANYCHAR_ML)  MOP_IN(OP_ANYCHAR_ML);
      DATA_ENSURE(1);
      n = enclen(encode, s, end);
      DATA_ENSURE(n);
      s += n;
      MOP_OUT;
      NEXT;

    CASE(OP_ANYCHAR_STAR)  MOP_IN(OP_ANYCHAR_STAR);
      while (DATA_ENSURE_CHECK1) {
	STACK_PUSH_ALT(p, s, sprev, pkeep);
	n = enclen(encode, s, end);
	DATA_ENSURE(n);
	if (ONIGENC_IS_MBC_NEWLINE_EX(encode, s, str, end, option, 0))  goto fail;
	sprev = s;
	s += n;
      }
      MOP_OUT;
      NEXT;

    CASE(OP_ANYCHAR_ML_STAR)  MOP_IN(OP_ANYCHAR_ML_STAR);
      while (DATA_ENSURE_CHECK1) {
	STACK_PUSH_ALT(p, s, sprev, pkeep);
	n = enclen(encode, s, end);
	if (n > 1) {
	  DATA_ENSURE(n);
	  sprev = s;
	  s += n;
	}
	else {
	  sprev = s;
	  s++;
	}
      }
      MOP_OUT;
      NEXT;

    CASE(OP_ANYCHAR_STAR_PEEK_NEXT)  MOP_IN(OP_ANYCHAR_STAR_PEEK_NEXT);
      while (DATA_ENSURE_CHECK1) {
	if (*p == *s) {
	  STACK_PUSH_ALT(p + 1, s, sprev, pkeep);
	}
	n = enclen(encode, s, end);
	DATA_ENSURE(n);
	if (ONIGENC_IS_MBC_NEWLINE_EX(encode, s, str, end, option, 0))  goto fail;
	sprev = s;
	s += n;
      }
      p++;
      MOP_OUT;
      NEXT;

    CASE(OP_ANYCHAR_ML_STAR_PEEK_NEXT)MOP_IN(OP_ANYCHAR_ML_STAR_PEEK_NEXT);
      while (DATA_ENSURE_CHECK1) {
	if (*p == *s) {
	  STACK_PUSH_ALT(p + 1, s, sprev, pkeep);
	}
	n = enclen(encode, s, end);
	if (n > 1) {
	  DATA_ENSURE(n);
	  sprev = s;
	  s += n;
	}
	else {
	  sprev = s;
	  s++;
	}
      }
      p++;
      MOP_OUT;
      NEXT;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
    CASE(OP_STATE_CHECK_ANYCHAR_STAR)  MOP_IN(OP_STATE_CHECK_ANYCHAR_STAR);
      GET_STATE_CHECK_NUM_INC(mem, p);
      while (DATA_ENSURE_CHECK1) {
	STATE_CHECK_VAL(scv, mem);
	if (scv) goto fail;

	STACK_PUSH_ALT_WITH_STATE_CHECK(p, s, sprev, mem, pkeep);
	n = enclen(encode, s, end);
	DATA_ENSURE(n);
	if (ONIGENC_IS_MBC_NEWLINE_EX(encode, s, str, end, option, 0))  goto fail;
	sprev = s;
	s += n;
      }
      MOP_OUT;
      NEXT;

    CASE(OP_STATE_CHECK_ANYCHAR_ML_STAR)
      MOP_IN(OP_STATE_CHECK_ANYCHAR_ML_STAR);

      GET_STATE_CHECK_NUM_INC(mem, p);
      while (DATA_ENSURE_CHECK1) {
	STATE_CHECK_VAL(scv, mem);
	if (scv) goto fail;

	STACK_PUSH_ALT_WITH_STATE_CHECK(p, s, sprev, mem, pkeep);
	n = enclen(encode, s, end);
	if (n > 1) {
	  DATA_ENSURE(n);
	  sprev = s;
	  s += n;
	}
	else {
	  sprev = s;
	  s++;
	}
      }
      MOP_OUT;
      NEXT;
#endif /* USE_COMBINATION_EXPLOSION_CHECK */

    CASE(OP_WORD)  MOP_IN(OP_WORD);
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_WORD(encode, s, end))
	goto fail;

      s += enclen(encode, s, end);
      MOP_OUT;
      NEXT;

    CASE(OP_ASCII_WORD)  MOP_IN(OP_ASCII_WORD);
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_ASCII_WORD(encode, s, end))
	goto fail;

      s += enclen(encode, s, end);
      MOP_OUT;
      NEXT;

    CASE(OP_NOT_WORD)  MOP_IN(OP_NOT_WORD);
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_WORD(encode, s, end))
	goto fail;

      s += enclen(encode, s, end);
      MOP_OUT;
      NEXT;

    CASE(OP_NOT_ASCII_WORD)  MOP_IN(OP_NOT_ASCII_WORD);
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_ASCII_WORD(encode, s, end))
	goto fail;

      s += enclen(encode, s, end);
      MOP_OUT;
      NEXT;

    CASE(OP_WORD_BOUND)  MOP_IN(OP_WORD_BOUND);
      if (ON_STR_BEGIN(s)) {
	DATA_ENSURE(1);
	if (! ONIGENC_IS_MBC_WORD(encode, s, end))
	  goto fail;
      }
      else if (ON_STR_END(s)) {
	if (! ONIGENC_IS_MBC_WORD(encode, sprev, end))
	  goto fail;
      }
      else {
	if (ONIGENC_IS_MBC_WORD(encode, s, end)
	    == ONIGENC_IS_MBC_WORD(encode, sprev, end))
	  goto fail;
      }
      MOP_OUT;
      JUMP;

    CASE(OP_ASCII_WORD_BOUND)  MOP_IN(OP_ASCII_WORD_BOUND);
      if (ON_STR_BEGIN(s)) {
	DATA_ENSURE(1);
	if (! ONIGENC_IS_MBC_ASCII_WORD(encode, s, end))
	  goto fail;
      }
      else if (ON_STR_END(s)) {
	if (! ONIGENC_IS_MBC_ASCII_WORD(encode, sprev, end))
	  goto fail;
      }
      else {
	if (ONIGENC_IS_MBC_ASCII_WORD(encode, s, end)
	    == ONIGENC_IS_MBC_ASCII_WORD(encode, sprev, end))
	  goto fail;
      }
      MOP_OUT;
      JUMP;

    CASE(OP_NOT_WORD_BOUND)  MOP_IN(OP_NOT_WORD_BOUND);
      if (ON_STR_BEGIN(s)) {
	if (DATA_ENSURE_CHECK1 && ONIGENC_IS_MBC_WORD(encode, s, end))
	  goto fail;
      }
      else if (ON_STR_END(s)) {
	if (ONIGENC_IS_MBC_WORD(encode, sprev, end))
	  goto fail;
      }
      else {
	if (ONIGENC_IS_MBC_WORD(encode, s, end)
	    != ONIGENC_IS_MBC_WORD(encode, sprev, end))
	  goto fail;
      }
      MOP_OUT;
      JUMP;

    CASE(OP_NOT_ASCII_WORD_BOUND)  MOP_IN(OP_NOT_ASCII_WORD_BOUND);
      if (ON_STR_BEGIN(s)) {
	if (DATA_ENSURE_CHECK1 && ONIGENC_IS_MBC_ASCII_WORD(encode, s, end))
	  goto fail;
      }
      else if (ON_STR_END(s)) {
	if (ONIGENC_IS_MBC_ASCII_WORD(encode, sprev, end))
	  goto fail;
      }
      else {
	if (ONIGENC_IS_MBC_ASCII_WORD(encode, s, end)
	    != ONIGENC_IS_MBC_ASCII_WORD(encode, sprev, end))
	  goto fail;
      }
      MOP_OUT;
      JUMP;

#ifdef USE_WORD_BEGIN_END
    CASE(OP_WORD_BEGIN)  MOP_IN(OP_WORD_BEGIN);
      if (DATA_ENSURE_CHECK1 && ONIGENC_IS_MBC_WORD(encode, s, end)) {
	if (ON_STR_BEGIN(s) || !ONIGENC_IS_MBC_WORD(encode, sprev, end)) {
	  MOP_OUT;
	  JUMP;
	}
      }
      goto fail;
      NEXT;

    CASE(OP_ASCII_WORD_BEGIN)  MOP_IN(OP_ASCII_WORD_BEGIN);
      if (DATA_ENSURE_CHECK1 && ONIGENC_IS_MBC_ASCII_WORD(encode, s, end)) {
	if (ON_STR_BEGIN(s) || !ONIGENC_IS_MBC_ASCII_WORD(encode, sprev, end)) {
	  MOP_OUT;
	  JUMP;
	}
      }
      goto fail;
      NEXT;

    CASE(OP_WORD_END)  MOP_IN(OP_WORD_END);
      if (!ON_STR_BEGIN(s) && ONIGENC_IS_MBC_WORD(encode, sprev, end)) {
	if (ON_STR_END(s) || !ONIGENC_IS_MBC_WORD(encode, s, end)) {
	  MOP_OUT;
	  JUMP;
	}
      }
      goto fail;
      NEXT;

    CASE(OP_ASCII_WORD_END)  MOP_IN(OP_ASCII_WORD_END);
      if (!ON_STR_BEGIN(s) && ONIGENC_IS_MBC_ASCII_WORD(encode, sprev, end)) {
	if (ON_STR_END(s) || !ONIGENC_IS_MBC_ASCII_WORD(encode, s, end)) {
	  MOP_OUT;
	  JUMP;
	}
      }
      goto fail;
      NEXT;
#endif

    CASE(OP_BEGIN_BUF)  MOP_IN(OP_BEGIN_BUF);
      if (! ON_STR_BEGIN(s)) goto fail;
      if (IS_NOTBOS(msa->options)) goto fail;

      MOP_OUT;
      JUMP;

    CASE(OP_END_BUF)  MOP_IN(OP_END_BUF);
      if (! ON_STR_END(s)) goto fail;
      if (IS_NOTEOS(msa->options)) goto fail;

      MOP_OUT;
      JUMP;

    CASE(OP_BEGIN_LINE)  MOP_IN(OP_BEGIN_LINE);
    op_begin_line:
      if (ON_STR_BEGIN(s)) {
	if (IS_NOTBOL(msa->options)) goto fail;
	MOP_OUT;
	JUMP;
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, sprev, end)
#ifdef USE_CRNL_AS_LINE_TERMINATOR
		&& !(IS_NEWLINE_CRLF(option)
		     && ONIGENC_IS_MBC_CRNL(encode, sprev, end))
#endif
		&& !ON_STR_END(s)) {
	MOP_OUT;
	JUMP;
      }
      goto fail;
      NEXT;

    CASE(OP_END_LINE)  MOP_IN(OP_END_LINE);
      if (ON_STR_END(s)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
	if (IS_EMPTY_STR || !ONIGENC_IS_MBC_NEWLINE_EX(encode, sprev, str, end, option, 1)) {
#endif
	  if (IS_NOTEOL(msa->options)) goto fail;
	  MOP_OUT;
	  JUMP;
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
	}
#endif
      }
      else if (ONIGENC_IS_MBC_NEWLINE_EX(encode, s, str, end, option, 1)) {
	MOP_OUT;
	JUMP;
      }
      goto fail;
      NEXT;

    CASE(OP_SEMI_END_BUF)  MOP_IN(OP_SEMI_END_BUF);
      if (ON_STR_END(s)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
	if (IS_EMPTY_STR || !ONIGENC_IS_MBC_NEWLINE_EX(encode, sprev, str, end, option, 1)) {
#endif
	  if (IS_NOTEOL(msa->options)) goto fail;
	  MOP_OUT;
	  JUMP;
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
	}
#endif
      }
      else if (ONIGENC_IS_MBC_NEWLINE_EX(encode, s, str, end, option, 1)) {
	UChar* ss = s + enclen(encode, s, end);
	if (ON_STR_END(ss)) {
	  MOP_OUT;
	  JUMP;
	}
#ifdef USE_CRNL_AS_LINE_TERMINATOR
	else if (IS_NEWLINE_CRLF(option)
	    && ONIGENC_IS_MBC_CRNL(encode, s, end)) {
	  ss += enclen(encode, ss, end);
	  if (ON_STR_END(ss)) {
	    MOP_OUT;
	    JUMP;
	  }
	}
#endif
      }
      goto fail;
      NEXT;

    CASE(OP_BEGIN_POSITION)  MOP_IN(OP_BEGIN_POSITION);
      if (s != msa->gpos)
	goto fail;

      MOP_OUT;
      JUMP;

    CASE(OP_BEGIN_POS_OR_LINE)  MOP_IN(OP_BEGIN_POS_OR_LINE);
      if (s != msa->gpos)
	goto op_begin_line;

      MOP_OUT;
      JUMP;

    CASE(OP_MEMORY_START_PUSH)  MOP_IN(OP_MEMORY_START_PUSH);
      GET_MEMNUM_INC(mem, p);
      STACK_PUSH_MEM_START(mem, s);
      MOP_OUT;
      JUMP;

    CASE(OP_MEMORY_START)  MOP_IN(OP_MEMORY_START);
      GET_MEMNUM_INC(mem, p);
      mem_start_stk[mem] = (OnigStackIndex )((void* )s);
      MOP_OUT;
      JUMP;

    CASE(OP_MEMORY_END_PUSH)  MOP_IN(OP_MEMORY_END_PUSH);
      GET_MEMNUM_INC(mem, p);
      STACK_PUSH_MEM_END(mem, s);
      MOP_OUT;
      JUMP;

    CASE(OP_MEMORY_END)  MOP_IN(OP_MEMORY_END);
      GET_MEMNUM_INC(mem, p);
      mem_end_stk[mem] = (OnigStackIndex )((void* )s);
      MOP_OUT;
      JUMP;

    CASE(OP_KEEP)  MOP_IN(OP_KEEP);
      pkeep = s;
      MOP_OUT;
      JUMP;

#ifdef USE_SUBEXP_CALL
    CASE(OP_MEMORY_END_PUSH_REC)  MOP_IN(OP_MEMORY_END_PUSH_REC);
      GET_MEMNUM_INC(mem, p);
      STACK_GET_MEM_START(mem, stkp); /* should be before push mem-end. */
      STACK_PUSH_MEM_END(mem, s);
      mem_start_stk[mem] = GET_STACK_INDEX(stkp);
      MOP_OUT;
      JUMP;

    CASE(OP_MEMORY_END_REC)  MOP_IN(OP_MEMORY_END_REC);
      GET_MEMNUM_INC(mem, p);
      mem_end_stk[mem] = (OnigStackIndex )((void* )s);
      STACK_GET_MEM_START(mem, stkp);

      if (BIT_STATUS_AT(reg->bt_mem_start, mem))
	mem_start_stk[mem] = GET_STACK_INDEX(stkp);
      else
	mem_start_stk[mem] = (OnigStackIndex )((void* )stkp->u.mem.pstr);

      STACK_PUSH_MEM_END_MARK(mem);
      MOP_OUT;
      JUMP;
#endif

    CASE(OP_BACKREF1)  MOP_IN(OP_BACKREF1);
      mem = 1;
      goto backref;
      NEXT;

    CASE(OP_BACKREF2)  MOP_IN(OP_BACKREF2);
      mem = 2;
      goto backref;
      NEXT;

    CASE(OP_BACKREFN)  MOP_IN(OP_BACKREFN);
      GET_MEMNUM_INC(mem, p);
    backref:
      {
	int len;
	UChar *pstart, *pend;

	/* if you want to remove following line,
	   you should check in parse and compile time. */
	if (mem > num_mem) goto fail;
	if (mem_end_stk[mem]   == INVALID_STACK_INDEX) goto fail;
	if (mem_start_stk[mem] == INVALID_STACK_INDEX) goto fail;

	if (BIT_STATUS_AT(reg->bt_mem_start, mem))
	  pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
	else
	  pstart = (UChar* )((void* )mem_start_stk[mem]);

	pend = (BIT_STATUS_AT(reg->bt_mem_end, mem)
		? STACK_AT(mem_end_stk[mem])->u.mem.pstr
		: (UChar* )((void* )mem_end_stk[mem]));
	n = pend - pstart;
	DATA_ENSURE(n);
	sprev = s;
	STRING_CMP(pstart, s, n);
	while (sprev + (len = enclen(encode, sprev, end)) < s)
	  sprev += len;

	MOP_OUT;
	JUMP;
      }

    CASE(OP_BACKREFN_IC)  MOP_IN(OP_BACKREFN_IC);
      GET_MEMNUM_INC(mem, p);
      {
	int len;
	UChar *pstart, *pend;

	/* if you want to remove following line,
	   you should check in parse and compile time. */
	if (mem > num_mem) goto fail;
	if (mem_end_stk[mem]   == INVALID_STACK_INDEX) goto fail;
	if (mem_start_stk[mem] == INVALID_STACK_INDEX) goto fail;

	if (BIT_STATUS_AT(reg->bt_mem_start, mem))
	  pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
	else
	  pstart = (UChar* )((void* )mem_start_stk[mem]);

	pend = (BIT_STATUS_AT(reg->bt_mem_end, mem)
		? STACK_AT(mem_end_stk[mem])->u.mem.pstr
		: (UChar* )((void* )mem_end_stk[mem]));
	n = pend - pstart;
	DATA_ENSURE(n);
	sprev = s;
	STRING_CMP_IC(case_fold_flag, pstart, &s, (int)n, end);
	while (sprev + (len = enclen(encode, sprev, end)) < s)
	  sprev += len;

	MOP_OUT;
	JUMP;
      }
      NEXT;

    CASE(OP_BACKREF_MULTI)  MOP_IN(OP_BACKREF_MULTI);
      {
	int len, is_fail;
	UChar *pstart, *pend, *swork;

	GET_LENGTH_INC(tlen, p);
	for (i = 0; i < tlen; i++) {
	  GET_MEMNUM_INC(mem, p);

	  if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
	  if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;

	  if (BIT_STATUS_AT(reg->bt_mem_start, mem))
	    pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
	  else
	    pstart = (UChar* )((void* )mem_start_stk[mem]);

	  pend = (BIT_STATUS_AT(reg->bt_mem_end, mem)
		  ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
		  : (UChar* )((void* )mem_end_stk[mem]));
	  n = pend - pstart;
	  DATA_ENSURE(n);
	  sprev = s;
	  swork = s;
	  STRING_CMP_VALUE(pstart, swork, n, is_fail);
	  if (is_fail) continue;
	  s = swork;
	  while (sprev + (len = enclen(encode, sprev, end)) < s)
	    sprev += len;

	  p += (SIZE_MEMNUM * (tlen - i - 1));
	  break; /* success */
	}
	if (i == tlen) goto fail;
	MOP_OUT;
	JUMP;
      }
      NEXT;

    CASE(OP_BACKREF_MULTI_IC)  MOP_IN(OP_BACKREF_MULTI_IC);
      {
	int len, is_fail;
	UChar *pstart, *pend, *swork;

	GET_LENGTH_INC(tlen, p);
	for (i = 0; i < tlen; i++) {
	  GET_MEMNUM_INC(mem, p);

	  if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
	  if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;

	  if (BIT_STATUS_AT(reg->bt_mem_start, mem))
	    pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
	  else
	    pstart = (UChar* )((void* )mem_start_stk[mem]);

	  pend = (BIT_STATUS_AT(reg->bt_mem_end, mem)
		  ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
		  : (UChar* )((void* )mem_end_stk[mem]));
	  n = pend - pstart;
	  DATA_ENSURE(n);
	  sprev = s;
	  swork = s;
	  STRING_CMP_VALUE_IC(case_fold_flag, pstart, &swork, n, end, is_fail);
	  if (is_fail) continue;
	  s = swork;
	  while (sprev + (len = enclen(encode, sprev, end)) < s)
	    sprev += len;

	  p += (SIZE_MEMNUM * (tlen - i - 1));
	  break; /* success */
	}
	if (i == tlen) goto fail;
	MOP_OUT;
	JUMP;
      }

#ifdef USE_BACKREF_WITH_LEVEL
    CASE(OP_BACKREF_WITH_LEVEL)
      {
	int len;
	OnigOptionType ic;
	LengthType level;

	GET_OPTION_INC(ic,    p);
	GET_LENGTH_INC(level, p);
	GET_LENGTH_INC(tlen,  p);

	sprev = s;
	if (backref_match_at_nested_level(reg, stk, stk_base, ic,
		  case_fold_flag, (int )level, (int )tlen, p, &s, end)) {
	  while (sprev + (len = enclen(encode, sprev, end)) < s)
	    sprev += len;

	  p += (SIZE_MEMNUM * tlen);
	}
	else
	  goto fail;

	MOP_OUT;
	JUMP;
      }

#endif

#if 0   /* no need: IS_DYNAMIC_OPTION() == 0 */
    CASE(OP_SET_OPTION_PUSH)  MOP_IN(OP_SET_OPTION_PUSH);
      GET_OPTION_INC(option, p);
      STACK_PUSH_ALT(p, s, sprev, pkeep);
      p += SIZE_OP_SET_OPTION + SIZE_OP_FAIL;
      MOP_OUT;
      JUMP;

    CASE(OP_SET_OPTION)  MOP_IN(OP_SET_OPTION);
      GET_OPTION_INC(option, p);
      MOP_OUT;
      JUMP;
#endif

    CASE(OP_NULL_CHECK_START)  MOP_IN(OP_NULL_CHECK_START);
      GET_MEMNUM_INC(mem, p);    /* mem: null check id */
      STACK_PUSH_NULL_CHECK_START(mem, s);
      MOP_OUT;
      JUMP;

    CASE(OP_NULL_CHECK_END)  MOP_IN(OP_NULL_CHECK_END);
      {
	int isnull;

	GET_MEMNUM_INC(mem, p); /* mem: null check id */
	STACK_NULL_CHECK(isnull, mem, s);
	if (isnull) {
#ifdef ONIG_DEBUG_MATCH
	  fprintf(stderr, "NULL_CHECK_END: skip  id:%d, s:%"PRIdPTR" (%p)\n",
		  (int )mem, (intptr_t )s, s);
#endif
	null_check_found:
	  /* empty loop founded, skip next instruction */
	  switch (*p++) {
	  case OP_JUMP:
	  case OP_PUSH:
	    p += SIZE_RELADDR;
	    break;
	  case OP_REPEAT_INC:
	  case OP_REPEAT_INC_NG:
	  case OP_REPEAT_INC_SG:
	  case OP_REPEAT_INC_NG_SG:
	    p += SIZE_MEMNUM;
	    break;
	  default:
	    goto unexpected_bytecode_error;
	    break;
	  }
	}
      }
      MOP_OUT;
      JUMP;

#ifdef USE_MONOMANIAC_CHECK_CAPTURES_IN_ENDLESS_REPEAT
    CASE(OP_NULL_CHECK_END_MEMST)  MOP_IN(OP_NULL_CHECK_END_MEMST);
      {
	int isnull;

	GET_MEMNUM_INC(mem, p); /* mem: null check id */
	STACK_NULL_CHECK_MEMST(isnull, mem, s, reg);
	if (isnull) {
# ifdef ONIG_DEBUG_MATCH
	  fprintf(stderr, "NULL_CHECK_END_MEMST: skip  id:%d, s:%"PRIdPTR" (%p)\n",
		  (int )mem, (intptr_t )s, s);
# endif
	  if (isnull == -1) goto fail;
	  goto null_check_found;
	}
      }
      MOP_OUT;
      JUMP;
#endif

#ifdef USE_SUBEXP_CALL
    CASE(OP_NULL_CHECK_END_MEMST_PUSH)
      MOP_IN(OP_NULL_CHECK_END_MEMST_PUSH);
      {
	int isnull;

	GET_MEMNUM_INC(mem, p); /* mem: null check id */
# ifdef USE_MONOMANIAC_CHECK_CAPTURES_IN_ENDLESS_REPEAT
	STACK_NULL_CHECK_MEMST_REC(isnull, mem, s, reg);
# else
	STACK_NULL_CHECK_REC(isnull, mem, s);
# endif
	if (isnull) {
# ifdef ONIG_DEBUG_MATCH
	  fprintf(stderr, "NULL_CHECK_END_MEMST_PUSH: skip  id:%d, s:%"PRIdPTR" (%p)\n",
		  (int )mem, (intptr_t )s, s);
# endif
	  if (isnull == -1) goto fail;
	  goto null_check_found;
	}
	else {
	  STACK_PUSH_NULL_CHECK_END(mem);
	}
      }
      MOP_OUT;
      JUMP;
#endif

    CASE(OP_JUMP)  MOP_IN(OP_JUMP);
      GET_RELADDR_INC(addr, p);
      p += addr;
      MOP_OUT;
      CHECK_INTERRUPT_IN_MATCH_AT;
      JUMP;

    CASE(OP_PUSH)  MOP_IN(OP_PUSH);
      GET_RELADDR_INC(addr, p);
      STACK_PUSH_ALT(p + addr, s, sprev, pkeep);
      MOP_OUT;
      JUMP;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
    CASE(OP_STATE_CHECK_PUSH)  MOP_IN(OP_STATE_CHECK_PUSH);
      GET_STATE_CHECK_NUM_INC(mem, p);
      STATE_CHECK_VAL(scv, mem);
      if (scv) goto fail;

      GET_RELADDR_INC(addr, p);
      STACK_PUSH_ALT_WITH_STATE_CHECK(p + addr, s, sprev, mem, pkeep);
      MOP_OUT;
      JUMP;

    CASE(OP_STATE_CHECK_PUSH_OR_JUMP)  MOP_IN(OP_STATE_CHECK_PUSH_OR_JUMP);
      GET_STATE_CHECK_NUM_INC(mem, p);
      GET_RELADDR_INC(addr, p);
      STATE_CHECK_VAL(scv, mem);
      if (scv) {
	p += addr;
      }
      else {
	STACK_PUSH_ALT_WITH_STATE_CHECK(p + addr, s, sprev, mem, pkeep);
      }
      MOP_OUT;
      JUMP;

    CASE(OP_STATE_CHECK)  MOP_IN(OP_STATE_CHECK);
      GET_STATE_CHECK_NUM_INC(mem, p);
      STATE_CHECK_VAL(scv, mem);
      if (scv) goto fail;

      STACK_PUSH_STATE_CHECK(s, mem);
      MOP_OUT;
      JUMP;
#endif /* USE_COMBINATION_EXPLOSION_CHECK */

    CASE(OP_POP)  MOP_IN(OP_POP);
      STACK_POP_ONE;
      MOP_OUT;
      JUMP;

#ifdef USE_OP_PUSH_OR_JUMP_EXACT
    CASE(OP_PUSH_OR_JUMP_EXACT1)  MOP_IN(OP_PUSH_OR_JUMP_EXACT1);
      GET_RELADDR_INC(addr, p);
      if (*p == *s && DATA_ENSURE_CHECK1) {
	p++;
	STACK_PUSH_ALT(p + addr, s, sprev, pkeep);
	MOP_OUT;
	JUMP;
      }
      p += (addr + 1);
      MOP_OUT;
      JUMP;
#endif

    CASE(OP_PUSH_IF_PEEK_NEXT)  MOP_IN(OP_PUSH_IF_PEEK_NEXT);
      GET_RELADDR_INC(addr, p);
      if (*p == *s) {
	p++;
	STACK_PUSH_ALT(p + addr, s, sprev, pkeep);
	MOP_OUT;
	JUMP;
      }
      p++;
      MOP_OUT;
      JUMP;

    CASE(OP_REPEAT)  MOP_IN(OP_REPEAT);
      {
	GET_MEMNUM_INC(mem, p);    /* mem: OP_REPEAT ID */
	GET_RELADDR_INC(addr, p);

	STACK_ENSURE(1);
	repeat_stk[mem] = GET_STACK_INDEX(stk);
	STACK_PUSH_REPEAT(mem, p);

	if (reg->repeat_range[mem].lower == 0) {
	  STACK_PUSH_ALT(p + addr, s, sprev, pkeep);
	}
      }
      MOP_OUT;
      JUMP;

    CASE(OP_REPEAT_NG)  MOP_IN(OP_REPEAT_NG);
      {
	GET_MEMNUM_INC(mem, p);    /* mem: OP_REPEAT ID */
	GET_RELADDR_INC(addr, p);

	STACK_ENSURE(1);
	repeat_stk[mem] = GET_STACK_INDEX(stk);
	STACK_PUSH_REPEAT(mem, p);

	if (reg->repeat_range[mem].lower == 0) {
	  STACK_PUSH_ALT(p, s, sprev, pkeep);
	  p += addr;
	}
      }
      MOP_OUT;
      JUMP;

    CASE(OP_REPEAT_INC)  MOP_IN(OP_REPEAT_INC);
      GET_MEMNUM_INC(mem, p); /* mem: OP_REPEAT ID */
      si = repeat_stk[mem];
      stkp = STACK_AT(si);

    repeat_inc:
      stkp->u.repeat.count++;
      if (stkp->u.repeat.count >= reg->repeat_range[mem].upper) {
	/* end of repeat. Nothing to do. */
      }
      else if (stkp->u.repeat.count >= reg->repeat_range[mem].lower) {
	STACK_PUSH_ALT(p, s, sprev, pkeep);
	p = STACK_AT(si)->u.repeat.pcode; /* Don't use stkp after PUSH. */
      }
      else {
	p = stkp->u.repeat.pcode;
      }
      STACK_PUSH_REPEAT_INC(si);
      MOP_OUT;
      CHECK_INTERRUPT_IN_MATCH_AT;
      JUMP;

    CASE(OP_REPEAT_INC_SG)  MOP_IN(OP_REPEAT_INC_SG);
      GET_MEMNUM_INC(mem, p); /* mem: OP_REPEAT ID */
      STACK_GET_REPEAT(mem, stkp);
      si = GET_STACK_INDEX(stkp);
      goto repeat_inc;
      NEXT;

    CASE(OP_REPEAT_INC_NG)  MOP_IN(OP_REPEAT_INC_NG);
      GET_MEMNUM_INC(mem, p); /* mem: OP_REPEAT ID */
      si = repeat_stk[mem];
      stkp = STACK_AT(si);

    repeat_inc_ng:
      stkp->u.repeat.count++;
      if (stkp->u.repeat.count < reg->repeat_range[mem].upper) {
	if (stkp->u.repeat.count >= reg->repeat_range[mem].lower) {
	  UChar* pcode = stkp->u.repeat.pcode;

	  STACK_PUSH_REPEAT_INC(si);
	  STACK_PUSH_ALT(pcode, s, sprev, pkeep);
	}
	else {
	  p = stkp->u.repeat.pcode;
	  STACK_PUSH_REPEAT_INC(si);
	}
      }
      else if (stkp->u.repeat.count == reg->repeat_range[mem].upper) {
	STACK_PUSH_REPEAT_INC(si);
      }
      MOP_OUT;
      CHECK_INTERRUPT_IN_MATCH_AT;
      JUMP;

    CASE(OP_REPEAT_INC_NG_SG)  MOP_IN(OP_REPEAT_INC_NG_SG);
      GET_MEMNUM_INC(mem, p); /* mem: OP_REPEAT ID */
      STACK_GET_REPEAT(mem, stkp);
      si = GET_STACK_INDEX(stkp);
      goto repeat_inc_ng;
      NEXT;

    CASE(OP_PUSH_POS)  MOP_IN(OP_PUSH_POS);
      STACK_PUSH_POS(s, sprev, pkeep);
      MOP_OUT;
      JUMP;

    CASE(OP_POP_POS)  MOP_IN(OP_POP_POS);
      {
	STACK_POS_END(stkp);
	s     = stkp->u.state.pstr;
	sprev = stkp->u.state.pstr_prev;
      }
      MOP_OUT;
      JUMP;

    CASE(OP_PUSH_POS_NOT)  MOP_IN(OP_PUSH_POS_NOT);
      GET_RELADDR_INC(addr, p);
      STACK_PUSH_POS_NOT(p + addr, s, sprev, pkeep);
      MOP_OUT;
      JUMP;

    CASE(OP_FAIL_POS)  MOP_IN(OP_FAIL_POS);
      STACK_POP_TIL_POS_NOT;
      goto fail;
      NEXT;

    CASE(OP_PUSH_STOP_BT)  MOP_IN(OP_PUSH_STOP_BT);
      STACK_PUSH_STOP_BT;
      MOP_OUT;
      JUMP;

    CASE(OP_POP_STOP_BT)  MOP_IN(OP_POP_STOP_BT);
      STACK_STOP_BT_END;
      MOP_OUT;
      JUMP;

    CASE(OP_LOOK_BEHIND)  MOP_IN(OP_LOOK_BEHIND);
      GET_LENGTH_INC(tlen, p);
      s = (UChar* )ONIGENC_STEP_BACK(encode, str, s, end, (int )tlen);
      if (IS_NULL(s)) goto fail;
      sprev = (UChar* )onigenc_get_prev_char_head(encode, str, s, end);
      MOP_OUT;
      JUMP;

    CASE(OP_PUSH_LOOK_BEHIND_NOT)  MOP_IN(OP_PUSH_LOOK_BEHIND_NOT);
      GET_RELADDR_INC(addr, p);
      GET_LENGTH_INC(tlen, p);
      q = (UChar* )ONIGENC_STEP_BACK(encode, str, s, end, (int )tlen);
      if (IS_NULL(q)) {
	/* too short case -> success. ex. /(?<!XXX)a/.match("a")
	   If you want to change to fail, replace following line. */
	p += addr;
	/* goto fail; */
      }
      else {
	STACK_PUSH_LOOK_BEHIND_NOT(p + addr, s, sprev, pkeep);
	s = q;
	sprev = (UChar* )onigenc_get_prev_char_head(encode, str, s, end);
      }
      MOP_OUT;
      JUMP;

    CASE(OP_FAIL_LOOK_BEHIND_NOT)  MOP_IN(OP_FAIL_LOOK_BEHIND_NOT);
      STACK_POP_TIL_LOOK_BEHIND_NOT;
      goto fail;
      NEXT;

#ifdef USE_SUBEXP_CALL
    CASE(OP_CALL)  MOP_IN(OP_CALL);
      GET_ABSADDR_INC(addr, p);
      STACK_PUSH_CALL_FRAME(p);
      p = reg->p + addr;
      MOP_OUT;
      JUMP;

    CASE(OP_RETURN)  MOP_IN(OP_RETURN);
      STACK_RETURN(p);
      STACK_PUSH_RETURN;
      MOP_OUT;
      JUMP;
#endif

    CASE(OP_CONDITION)  MOP_IN(OP_CONDITION);
      GET_MEMNUM_INC(mem, p);
      GET_RELADDR_INC(addr, p);
      if ((mem > num_mem) ||
	  (mem_end_stk[mem]   == INVALID_STACK_INDEX) ||
	  (mem_start_stk[mem] == INVALID_STACK_INDEX)) {
	p += addr;
      }
      MOP_OUT;
      JUMP;

    CASE(OP_FINISH)
      goto finish;
      NEXT;

    CASE(OP_FAIL)
      if (0) {
	/* fall */
      fail:
	MOP_OUT;
      }
      MOP_IN(OP_FAIL);
      STACK_POP;
      p     = stk->u.state.pcode;
      s     = stk->u.state.pstr;
      sprev = stk->u.state.pstr_prev;
      pkeep = stk->u.state.pkeep;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
      if (stk->u.state.state_check != 0) {
	stk->type = STK_STATE_CHECK_MARK;
	stk++;
      }
#endif

      MOP_OUT;
      JUMP;

    DEFAULT
      goto bytecode_error;
  } VM_LOOP_END

 finish:
  STACK_SAVE;
  if (xmalloc_base) xfree(xmalloc_base);
  return best_len;

#ifdef ONIG_DEBUG
 stack_error:
  STACK_SAVE;
  if (xmalloc_base) xfree(xmalloc_base);
  return ONIGERR_STACK_BUG;
#endif

 bytecode_error:
  STACK_SAVE;
  if (xmalloc_base) xfree(xmalloc_base);
  return ONIGERR_UNDEFINED_BYTECODE;

 unexpected_bytecode_error:
  STACK_SAVE;
  if (xmalloc_base) xfree(xmalloc_base);
  return ONIGERR_UNEXPECTED_BYTECODE;
}