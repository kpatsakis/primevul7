unix_s_socketpair(int argc, VALUE *argv, VALUE klass)
{
    VALUE domain, type, protocol;
    VALUE args[3];

    domain = INT2FIX(PF_UNIX);
    rb_scan_args(argc, argv, "02", &type, &protocol);
    if (argc == 0)
	type = INT2FIX(SOCK_STREAM);
    if (argc <= 1)
	protocol = INT2FIX(0);

    args[0] = domain;
    args[1] = type;
    args[2] = protocol;

    return rsock_sock_s_socketpair(3, args, klass);
}