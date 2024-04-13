copy_redo(int old_redo)
{
    int	    c;

    while ((c = read_redo(FALSE, old_redo)) != NUL)
	add_char_buff(&readbuf2, c);
}