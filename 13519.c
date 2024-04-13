replace_character(int c)
{
    int n = State;

    State = REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}