static inline bool tok_type(const Token *x, enum pp_token_type t)
{
    return x && x->type == t;
}