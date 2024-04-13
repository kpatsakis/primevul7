match_named_captures(VALUE match)
{
    VALUE hash;
    struct MEMO *memo;

    match_check(match);

    hash = rb_hash_new();
    memo = MEMO_NEW(hash, match, 0);

    onig_foreach_name(RREGEXP(RMATCH(match)->regexp)->ptr, match_named_captures_iter, (void*)memo);

    return hash;
}