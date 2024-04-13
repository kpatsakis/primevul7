spell_expand_check_cap(colnr_T col)
{
    spell_expand_need_cap = check_need_cap(curwin->w_cursor.lnum, col);
}