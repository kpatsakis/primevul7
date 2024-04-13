static int read_and_execute(bool interactive)
{
#if defined(__WIN__)
  String tmpbuf;
  String buffer;
#endif

  char	*line= NULL;
  char	in_string=0;
  ulong line_number=0;
  bool ml_comment= 0;  
  COMMANDS *com;
  status.exit_status=1;

  for (;;)
  {
    if (!interactive)
    {
      line=batch_readline(status.line_buff);
      /*
        Skip UTF8 Byte Order Marker (BOM) 0xEFBBBF.
        Editors like "notepad" put this marker in
        the very beginning of a text file when
        you save the file using "Unicode UTF-8" format.
      */
      if (line && !line_number &&
           (uchar) line[0] == 0xEF &&
           (uchar) line[1] == 0xBB &&
           (uchar) line[2] == 0xBF)
        line+= 3;
      line_number++;
      if (!glob_buffer.length())
	status.query_start_line=line_number;
    }
    else
    {
      char *prompt= (char*) (ml_comment ? "   /*> " :
                             glob_buffer.is_empty() ?  construct_prompt() :
			     !in_string ? "    -> " :
			     in_string == '\'' ?
			     "    '> " : (in_string == '`' ?
			     "    `> " :
			     "    \"> "));
      if (opt_outfile && glob_buffer.is_empty())
	fflush(OUTFILE);

#if defined(__WIN__)
      tee_fputs(prompt, stdout);
      if (!tmpbuf.is_alloced())
        tmpbuf.alloc(65535);
      tmpbuf.length(0);
      buffer.length(0);
      size_t clen;
      do
      {
	line= my_cgets((char*)tmpbuf.ptr(), tmpbuf.alloced_length()-1, &clen);
        buffer.append(line, clen);
        /* 
           if we got buffer fully filled than there is a chance that
           something else is still in console input buffer
        */
      } while (tmpbuf.alloced_length() <= clen);
      /* 
        An empty line is returned from my_cgets when there's error reading :
        Ctrl-c for example
      */
      if (line)
        line= buffer.c_ptr();
#else
      if (opt_outfile)
	fputs(prompt, OUTFILE);
      /*
        free the previous entered line.
        Note: my_free() cannot be used here as the memory was allocated under
        the readline/libedit library.
      */
      if (line)
        free(line);
      line= readline(prompt);
#endif /* defined(__WIN__) */

      /*
        When Ctrl+d or Ctrl+z is pressed, the line may be NULL on some OS
        which may cause coredump.
      */
      if (opt_outfile && line)
	fprintf(OUTFILE, "%s\n", line);
    }
    // End of file or system error
    if (!line)
    {
      if (status.line_buff && status.line_buff->error)
        status.exit_status= 1;
      else
        status.exit_status= 0;
      break;
    }

    /*
      Check if line is a mysql command line
      (We want to allow help, print and clear anywhere at line start
    */
    if ((named_cmds || glob_buffer.is_empty())
	&& !ml_comment && !in_string && (com=find_command(line,0)))
    {
      if ((*com->func)(&glob_buffer,line) > 0)
	break;
      if (glob_buffer.is_empty())		// If buffer was emptied
	in_string=0;
#ifdef HAVE_READLINE
      if (interactive && status.add_to_history && not_in_history(line))
	add_history(line);
#endif
      continue;
    }
    if (add_line(glob_buffer, line, &in_string, &ml_comment,
                 status.line_buff ? status.line_buff->truncated : 0))
      break;
  }
  /* if in batch mode, send last query even if it doesn't end with \g or go */

  if (!interactive && !status.exit_status)
  {
    remove_cntrl(glob_buffer);
    if (!glob_buffer.is_empty())
    {
      status.exit_status=1;
      if (com_go(&glob_buffer,line) <= 0)
	status.exit_status=0;
    }
  }

#if defined(__WIN__)
  buffer.free();
  tmpbuf.free();
#else
  if (interactive)
    /*
      free the last entered line.
      Note: my_free() cannot be used here as the memory was allocated under
      the readline/libedit library.
    */
    free(line);
#endif


  return status.exit_status;
}