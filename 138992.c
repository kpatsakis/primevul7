vungetc(int c)
{
    old_char = c;
    old_mod_mask = mod_mask;
#ifdef FEAT_MOUSE
    old_mouse_row = mouse_row;
    old_mouse_col = mouse_col;
#endif
}