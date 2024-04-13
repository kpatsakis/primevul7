usm_remove_user(struct usmUser *user)
{
    return usm_remove_user_from_list(user, &userList);
}