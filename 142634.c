usm_add_user(struct usmUser *user)
{
    struct usmUser *uptr;
    uptr = usm_add_user_to_list(user, userList);
    if (uptr != NULL)
        userList = uptr;
    return uptr;
}