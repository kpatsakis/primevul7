skip_comment(
    char_u   *line,
    int      process,
    int	     include_space,
    int      *is_comment)
{
    char_u *comment_flags = NULL;
    int    lead_len;
    int    leader_offset = get_last_leader_offset(line, &comment_flags);

    *is_comment = FALSE;
    if (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	while (*comment_flags)
	{
	    if (*comment_flags == COM_END
		    || *comment_flags == ':')
		break;
	    ++comment_flags;
	}
	if (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    if (process == FALSE)
	return line;

    lead_len = get_leader_len(line, &comment_flags, FALSE, include_space);

    if (lead_len == 0)
	return line;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    while (*comment_flags)
    {
	if (*comment_flags == COM_END
		|| *comment_flags == ':')
	    break;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    if (*comment_flags == ':' || *comment_flags == NUL)
	line += lead_len;

    return line;
}