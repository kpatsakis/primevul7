static int imap_compile_search (const pattern_t* pat, BUFFER* buf)
{
  if (! do_search (pat, 0))
    return 0;

  if (pat->not)
    mutt_buffer_addstr (buf, "NOT ");

  if (pat->child)
  {
    int clauses;

    if ((clauses = do_search (pat->child, 1)) > 0)
    {
      const pattern_t* clause = pat->child;

      mutt_buffer_addch (buf, '(');

      while (clauses)
      {
        if (do_search (clause, 0))
        {
          if (pat->op == MUTT_OR && clauses > 1)
            mutt_buffer_addstr (buf, "OR ");
          clauses--;

          if (imap_compile_search (clause, buf) < 0)
            return -1;

          if (clauses)
            mutt_buffer_addch (buf, ' ');

        }
        clause = clause->next;
      }

      mutt_buffer_addch (buf, ')');
    }
  }
  else
  {
    char term[STRING];
    char *delim;

    switch (pat->op)
    {
      case MUTT_HEADER:
        mutt_buffer_addstr (buf, "HEADER ");

        /* extract header name */
        if (! (delim = strchr (pat->p.str, ':')))
        {
          mutt_error (_("Header search without header name: %s"), pat->p.str);
          return -1;
        }
        *delim = '\0';
        imap_quote_string (term, sizeof (term), pat->p.str);
        mutt_buffer_addstr (buf, term);
        mutt_buffer_addch (buf, ' ');

        /* and field */
        *delim = ':';
        delim++;
        SKIPWS(delim);
        imap_quote_string (term, sizeof (term), delim);
        mutt_buffer_addstr (buf, term);
        break;
      case MUTT_BODY:
        mutt_buffer_addstr (buf, "BODY ");
        imap_quote_string (term, sizeof (term), pat->p.str);
        mutt_buffer_addstr (buf, term);
        break;
      case MUTT_WHOLE_MSG:
        mutt_buffer_addstr (buf, "TEXT ");
        imap_quote_string (term, sizeof (term), pat->p.str);
        mutt_buffer_addstr (buf, term);
        break;
    }
  }

  return 0;
}