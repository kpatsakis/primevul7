setup_tree(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int type;
  int r = 0;
  int in_root = state & IN_ROOT;

  state &= ~IN_ROOT;
restart:
  type = NTYPE(node);
  switch (type) {
  case NT_LIST:
    {
      Node* prev = NULL_NODE;
      int prev_in_root = 0;
      state |= in_root;
      do {
	r = setup_tree(NCAR(node), reg, state, env);
	if (IS_NOT_NULL(prev) && r == 0) {
	  r = next_setup(prev, NCAR(node), prev_in_root, reg);
	}
	prev = NCAR(node);
	prev_in_root = state & IN_ROOT;
	state &= ~IN_ROOT;
      } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    }
    break;

  case NT_ALT:
    do {
      r = setup_tree(NCAR(node), reg, (state | IN_ALT), env);
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;

  case NT_CCLASS:
    break;

  case NT_STR:
    if (IS_IGNORECASE(reg->options) && !NSTRING_IS_RAW(node)) {
      r = expand_case_fold_string(node, reg);
    }
    break;

  case NT_CTYPE:
  case NT_CANY:
    break;

#ifdef USE_SUBEXP_CALL
  case NT_CALL:
    break;
#endif

  case NT_BREF:
    {
      int i;
      int* p;
      Node** nodes = SCANENV_MEM_NODES(env);
      BRefNode* br = NBREF(node);
      p = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
	if (p[i] > env->num_mem)  return ONIGERR_INVALID_BACKREF;
	BIT_STATUS_ON_AT(env->backrefed_mem, p[i]);
	BIT_STATUS_ON_AT(env->bt_mem_start, p[i]);
#ifdef USE_BACKREF_WITH_LEVEL
	if (IS_BACKREF_NEST_LEVEL(br)) {
	  BIT_STATUS_ON_AT(env->bt_mem_end, p[i]);
	}
#endif
	SET_ENCLOSE_STATUS(nodes[p[i]], NST_MEM_BACKREFED);
      }
    }
    break;

  case NT_QTFR:
    {
      OnigDistance d;
      QtfrNode* qn = NQTFR(node);
      Node* target = qn->target;

      if ((state & IN_REPEAT) != 0) {
	qn->state |= NST_IN_REPEAT;
      }

      if (IS_REPEAT_INFINITE(qn->upper) || qn->upper >= 1) {
	r = get_min_match_length(target, &d, env);
	if (r) break;
	if (d == 0) {
	  qn->target_empty_info = NQ_TARGET_IS_EMPTY;
#ifdef USE_MONOMANIAC_CHECK_CAPTURES_IN_ENDLESS_REPEAT
	  r = quantifiers_memory_node_info(target);
	  if (r < 0) break;
	  if (r > 0) {
	    qn->target_empty_info = r;
	  }
#endif
#if 0
	  r = get_max_match_length(target, &d, env);
	  if (r == 0 && d == 0) {
	    /*  ()* ==> ()?, ()+ ==> ()  */
	    qn->upper = 1;
	    if (qn->lower > 1) qn->lower = 1;
	    if (NTYPE(target) == NT_STR) {
	      qn->upper = qn->lower = 0;  /* /(?:)+/ ==> // */
	    }
	  }
#endif
	}
      }

      state |= IN_REPEAT;
      if (qn->lower != qn->upper)
	state |= IN_VAR_REPEAT;
      r = setup_tree(target, reg, state, env);
      if (r) break;

      /* expand string */
#define EXPAND_STRING_MAX_LENGTH  100
      if (NTYPE(target) == NT_STR) {
	if (qn->lower > 1) {
	  int i, n = qn->lower;
	  OnigDistance len = NSTRING_LEN(target);
	  StrNode* sn = NSTR(target);
	  Node* np;

	  np = onig_node_new_str(sn->s, sn->end);
	  if (IS_NULL(np)) return ONIGERR_MEMORY;
	  NSTR(np)->flag = sn->flag;

	  for (i = 1; i < n && (i+1) * len <= EXPAND_STRING_MAX_LENGTH; i++) {
	    r = onig_node_str_cat(np, sn->s, sn->end);
	    if (r) {
	      onig_node_free(np);
	      return r;
	    }
	  }
	  if (i < qn->upper || IS_REPEAT_INFINITE(qn->upper)) {
	    Node *np1, *np2;

	    qn->lower -= i;
	    if (! IS_REPEAT_INFINITE(qn->upper))
	      qn->upper -= i;

	    np1 = onig_node_new_list(np, NULL);
	    if (IS_NULL(np1)) {
	      onig_node_free(np);
	      return ONIGERR_MEMORY;
	    }
	    swap_node(np1, node);
	    np2 = onig_node_list_add(node, np1);
	    if (IS_NULL(np2)) {
	      onig_node_free(np1);
	      return ONIGERR_MEMORY;
	    }
	  }
	  else {
	    swap_node(np, node);
	    onig_node_free(np);
	  }
	  break; /* break case NT_QTFR: */
	}
      }

#ifdef USE_OP_PUSH_OR_JUMP_EXACT
      if (qn->greedy && (qn->target_empty_info != 0)) {
	if (NTYPE(target) == NT_QTFR) {
	  QtfrNode* tqn = NQTFR(target);
	  if (IS_NOT_NULL(tqn->head_exact)) {
	    qn->head_exact  = tqn->head_exact;
	    tqn->head_exact = NULL;
	  }
	}
	else {
	  qn->head_exact = get_head_value_node(qn->target, 1, reg);
	}
      }
#endif
    }
    break;

  case NT_ENCLOSE:
    {
      EncloseNode* en = NENCLOSE(node);

      switch (en->type) {
      case ENCLOSE_OPTION:
	{
	  OnigOptionType options = reg->options;
	  state |= in_root;
	  reg->options = NENCLOSE(node)->option;
	  r = setup_tree(NENCLOSE(node)->target, reg, state, env);
	  reg->options = options;
	}
	break;

      case ENCLOSE_MEMORY:
	if ((state & (IN_ALT | IN_NOT | IN_VAR_REPEAT | IN_CALL)) != 0) {
	  BIT_STATUS_ON_AT(env->bt_mem_start, en->regnum);
	  /* SET_ENCLOSE_STATUS(node, NST_MEM_IN_ALT_NOT); */
	}
	if (IS_ENCLOSE_CALLED(en))
	  state |= IN_CALL;
	if (IS_ENCLOSE_RECURSION(en))
	  state |= IN_RECCALL;
	else if ((state & IN_RECCALL) != 0)
	  SET_CALL_RECURSION(node);
	r = setup_tree(en->target, reg, state, env);
	break;

      case ENCLOSE_STOP_BACKTRACK:
	{
	  Node* target = en->target;
	  r = setup_tree(target, reg, state, env);
	  if (NTYPE(target) == NT_QTFR) {
	    QtfrNode* tqn = NQTFR(target);
	    if (IS_REPEAT_INFINITE(tqn->upper) && tqn->lower <= 1 &&
		tqn->greedy != 0) {  /* (?>a*), a*+ etc... */
	      int qtype = NTYPE(tqn->target);
	      if (IS_NODE_TYPE_SIMPLE(qtype))
		SET_ENCLOSE_STATUS(node, NST_STOP_BT_SIMPLE_REPEAT);
	    }
	  }
	}
	break;

      case ENCLOSE_CONDITION:
#ifdef USE_NAMED_GROUP
	if (! IS_ENCLOSE_NAME_REF(NENCLOSE(node)) &&
	    env->num_named > 0 &&
	    IS_SYNTAX_BV(env->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
	    !ONIG_IS_OPTION_ON(env->option, ONIG_OPTION_CAPTURE_GROUP)) {
	  return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
	}
#endif
	if (NENCLOSE(node)->regnum > env->num_mem)
	  return ONIGERR_INVALID_BACKREF;
	r = setup_tree(NENCLOSE(node)->target, reg, state, env);
	break;
      }
    }
    break;

  case NT_ANCHOR:
    {
      AnchorNode* an = NANCHOR(node);

      switch (an->type) {
      case ANCHOR_PREC_READ:
	r = setup_tree(an->target, reg, state, env);
	break;
      case ANCHOR_PREC_READ_NOT:
	r = setup_tree(an->target, reg, (state | IN_NOT), env);
	break;

/* allowed node types in look-behind */
#define ALLOWED_TYPE_IN_LB  \
  ( BIT_NT_LIST | BIT_NT_ALT | BIT_NT_STR | BIT_NT_CCLASS | BIT_NT_CTYPE | \
    BIT_NT_CANY | BIT_NT_ANCHOR | BIT_NT_ENCLOSE | BIT_NT_QTFR | BIT_NT_CALL )

#define ALLOWED_ENCLOSE_IN_LB       ( ENCLOSE_MEMORY | ENCLOSE_OPTION )
#define ALLOWED_ENCLOSE_IN_LB_NOT   ENCLOSE_OPTION

#define ALLOWED_ANCHOR_IN_LB \
( ANCHOR_LOOK_BEHIND | ANCHOR_LOOK_BEHIND_NOT | ANCHOR_BEGIN_LINE | \
  ANCHOR_END_LINE | ANCHOR_BEGIN_BUF | ANCHOR_BEGIN_POSITION | ANCHOR_KEEP | \
  ANCHOR_WORD_BOUND | ANCHOR_NOT_WORD_BOUND | \
  ANCHOR_WORD_BEGIN | ANCHOR_WORD_END )
#define ALLOWED_ANCHOR_IN_LB_NOT \
( ANCHOR_LOOK_BEHIND | ANCHOR_LOOK_BEHIND_NOT | ANCHOR_BEGIN_LINE | \
  ANCHOR_END_LINE | ANCHOR_BEGIN_BUF | ANCHOR_BEGIN_POSITION | ANCHOR_KEEP | \
  ANCHOR_WORD_BOUND | ANCHOR_NOT_WORD_BOUND | \
  ANCHOR_WORD_BEGIN | ANCHOR_WORD_END )

      case ANCHOR_LOOK_BEHIND:
	{
	  r = check_type_tree(an->target, ALLOWED_TYPE_IN_LB,
			      ALLOWED_ENCLOSE_IN_LB, ALLOWED_ANCHOR_IN_LB);
	  if (r < 0) return r;
	  if (r > 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
	  if (NTYPE(node) != NT_ANCHOR) goto restart;
	  r = setup_tree(an->target, reg, state, env);
	  if (r != 0) return r;
	  r = setup_look_behind(node, reg, env);
	}
	break;

      case ANCHOR_LOOK_BEHIND_NOT:
	{
	  r = check_type_tree(an->target, ALLOWED_TYPE_IN_LB,
		      ALLOWED_ENCLOSE_IN_LB_NOT, ALLOWED_ANCHOR_IN_LB_NOT);
	  if (r < 0) return r;
	  if (r > 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
	  if (NTYPE(node) != NT_ANCHOR) goto restart;
	  r = setup_tree(an->target, reg, (state | IN_NOT), env);
	  if (r != 0) return r;
	  r = setup_look_behind(node, reg, env);
	}
	break;
      }
    }
    break;

  default:
    break;
  }

  return r;
}