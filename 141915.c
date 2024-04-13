int main(int argc, char **argv)
{
    int opt, timeout = 1800, family = AF_UNSPEC, hashlength = 0, hashmin = 0;
    int daemonize = FALSE, always_rewrite = FALSE;
    char *listen_addr = NULL, *forward_service = NULL, *reverse_service = NULL,
         *user = NULL, *domain = NULL, *chroot_dir = NULL;
    char separator = '=';
    char *secret_file = NULL, *pid_file = NULL;
    FILE *pf = NULL, *sf = NULL;
    struct passwd *pwd = NULL;
    char secretbuf[1024], *secret = NULL;
    char *tmp;
    time_t now;
    srs_t *srs;
    const char **excludes;
    size_t s1 = 0, s2 = 1;
    struct pollfd fds[4];
    size_t socket_count = 0, sc;
    int sockets[4] = {-1, -1, -1, -1};
    handle_t handler[4] = {0, 0, 0, 0};
    int fd, maxfd;

    excludes = (const char **)calloc(1, sizeof(char *));
    tmp = strrchr(argv[0], '/');
    if (tmp)
        self = strdup(tmp + 1);
    else
        self = strdup(argv[0]);

    while ((opt = getopt(argc, argv, "46d:a:l:f:r:s:n:N:u:t:p:c:X::ADhev"))
           != -1)
    {
        switch (opt)
        {
            case '?':
                return EXIT_FAILURE;
            case '4':
                family = AF_INET;
                break;
            case '6':
                family = AF_INET6;
                break;
            case 'd':
                domain = strdup(optarg);
                break;
            case 'a':
                separator = *optarg;
                break;
            case 'l':
                listen_addr = strdup(optarg);
                break;
            case 'f':
                forward_service = strdup(optarg);
                break;
            case 'r':
                reverse_service = strdup(optarg);
                break;
            case 't':
                timeout = atoi(optarg);
                break;
            case 's':
                secret_file = strdup(optarg);
                break;
            case 'n':
                hashlength = atoi(optarg);
                break;
            case 'N':
                hashmin = atoi(optarg);
                break;
            case 'p':
                pid_file = strdup(optarg);
                break;
            case 'u':
                user = strdup(optarg);
                break;
            case 'c':
                chroot_dir = strdup(optarg);
                break;
            case 'D':
                daemonize = TRUE;
                break;
            case 'A':
                always_rewrite = TRUE;
                break;
            case 'h':
                show_help();
                return EXIT_SUCCESS;
            case 'X':
                if (optarg != NULL)
                {
                    tmp = strtok(optarg, ",; \t\r\n");
                    while (tmp)
                    {
                        if (s1 + 1 >= s2)
                        {
                            s2 *= 2;
                            excludes = (const char **)realloc(
                                excludes, s2 * sizeof(char *));
                            if (excludes == NULL)
                            {
                                fprintf(stderr, "%s: Out of memory\n\n", self);
                                return EXIT_FAILURE;
                            }
                        }
                        excludes[s1++] = strdup(tmp);
                        tmp = strtok(NULL, ",; \t\r\n");
                    }
                    excludes[s1] = NULL;
                }
                break;
            case 'e':
                if (getenv("SRS_DOMAIN") != NULL)
                    domain = strdup(getenv("SRS_DOMAIN"));
                if (getenv("SRS_SEPARATOR") != NULL)
                    separator = *getenv("SRS_SEPARATOR");
                if (getenv("SRS_HASHLENGTH") != NULL)
                    hashlength = atoi(getenv("SRS_HASHLENGTH"));
                if (getenv("SRS_HASHMIN") != NULL)
                    hashmin = atoi(getenv("SRS_HASHMIN"));
                if (getenv("SRS_FORWARD_PORT") != NULL)
                    forward_service = strdup(getenv("SRS_FORWARD_PORT"));
                if (getenv("SRS_REVERSE_PORT") != NULL)
                    reverse_service = strdup(getenv("SRS_REVERSE_PORT"));
                if (getenv("SRS_TIMEOUT") != NULL)
                    timeout = atoi(getenv("SRS_TIMEOUT"));
                if (getenv("SRS_SECRET") != NULL)
                    secret_file = strdup(getenv("SRS_SECRET"));
                if (getenv("SRS_PID_FILE") != NULL)
                    pid_file = strdup(getenv("SRS_PID_FILE"));
                if (getenv("RUN_AS") != NULL)
                    user = strdup(getenv("RUN_AS"));
                if (getenv("CHROOT") != NULL)
                    chroot_dir = strdup(getenv("CHROOT"));
                if (getenv("SRS_EXCLUDE_DOMAINS") != NULL)
                {
                    tmp = strtok(getenv("SRS_EXCLUDE_DOMAINS"), ",; \t\r\n");
                    while (tmp)
                    {
                        if (s1 + 1 >= s2)
                        {
                            s2 *= 2;
                            excludes = (const char **)realloc(
                                excludes, s2 * sizeof(char *));
                            if (excludes == NULL)
                            {
                                fprintf(stderr, "%s: Out of memory\n\n", self);
                                return EXIT_FAILURE;
                            }
                        }
                        excludes[s1++] = strdup(tmp);
                        tmp = strtok(NULL, ",; \t\r\n");
                    }
                    excludes[s1] = NULL;
                }
                break;
            case 'v':
                fprintf(stdout, "%s\n", POSTSRSD_VERSION);
                return EXIT_SUCCESS;
        }
    }
    if (optind < argc)
    {
        fprintf(stderr, "%s: extra argument on command line: %s\n", self,
                argv[optind]);
        return EXIT_FAILURE;
    }
    if (domain == NULL || *domain == 0)
    {
        fprintf(stderr, "%s: You must set a home domain (-d)\n", self);
        return EXIT_FAILURE;
    }

    if (separator != '=' && separator != '+' && separator != '-')
    {
        fprintf(stderr, "%s: SRS separator character must be one of '=+-'\n",
                self);
        return EXIT_FAILURE;
    }
    if (forward_service == NULL)
        forward_service = strdup("10001");
    if (reverse_service == NULL)
        reverse_service = strdup("10002");

    /* Close all file descriptors (std ones will be closed later). */
    maxfd = sysconf(_SC_OPEN_MAX);
    for (fd = 3; fd < maxfd; fd++)
        close(fd);

    /* The stuff we do first may not be possible from within chroot or without
     * privileges */

    /* Open pid file for writing (the actual process ID is filled in later) */
    if (pid_file)
    {
        pf = fopen(pid_file, "w");
        if (pf == NULL)
        {
            fprintf(stderr, "%s: Cannot write PID: %s\n\n", self, pid_file);
            return EXIT_FAILURE;
        }
    }
    /* Read secret. The default installation makes this root accessible only. */
    if (secret_file != NULL)
    {
        sf = fopen(secret_file, "rb");
        if (sf == NULL)
        {
            fprintf(stderr, "%s: Cannot open file with secret: %s\n", self,
                    secret_file);
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "%s: You must set a secret (-s)\n", self);
        return EXIT_FAILURE;
    }
    /* Bind ports. May require privileges if the config specifies ports below
     * 1024 */
    sc = bind_service(listen_addr, forward_service, family,
                      &sockets[socket_count], 4 - socket_count);
    if (sc == 0)
        return EXIT_FAILURE;
    while (sc-- > 0)
        handler[socket_count++] = handle_forward;
    free(forward_service);
    sc = bind_service(listen_addr, reverse_service, family,
                      &sockets[socket_count], 4 - socket_count);
    if (sc == 0)
        return EXIT_FAILURE;
    while (sc-- > 0)
        handler[socket_count++] = handle_reverse;
    free(reverse_service);
    /* Open syslog now (NDELAY), because it may no longer be reachable from
     * chroot */
    openlog(self, LOG_PID | LOG_NDELAY, LOG_MAIL);
    /* Force loading of timezone info (suggested by patrickdk77) */
    now = time(NULL);
    localtime(&now);
    /* We also have to lookup the uid of the unprivileged user before the
     * chroot. */
    if (user)
    {
        errno = 0;
        pwd = getpwnam(user);
        if (pwd == NULL)
        {
            if (errno != 0)
                fprintf(stderr, "%s: Failed to lookup user: %s\n", self,
                        strerror(errno));
            else
                fprintf(stderr, "%s: No such user: %s\n", self, user);
            return EXIT_FAILURE;
        }
    }
    /* Now we can chroot, which again requires root privileges */
    if (chroot_dir)
    {
        if (chdir(chroot_dir) < 0)
        {
            fprintf(stderr, "%s: Cannot change to chroot: %s\n", self,
                    strerror(errno));
            return EXIT_FAILURE;
        }
        if (chroot(chroot_dir) < 0)
        {
            fprintf(stderr, "%s: Failed to enable chroot: %s\n", self,
                    strerror(errno));
            return EXIT_FAILURE;
        }
    }
    /* Finally, we revert to the unprivileged user */
    if (pwd)
    {
        if (setgid(pwd->pw_gid) < 0)
        {
            fprintf(stderr, "%s: Failed to switch group id: %s\n", self,
                    strerror(errno));
            return EXIT_FAILURE;
        }
        if (setuid(pwd->pw_uid) < 0)
        {
            fprintf(stderr, "%s: Failed to switch user id: %s\n", self,
                    strerror(errno));
            return EXIT_FAILURE;
        }
    }
    /* Standard double fork technique to disavow all knowledge about the
     * controlling terminal */
    if (daemonize)
    {
        close(0);
        close(1);
        close(2);
        if (fork() != 0)
            return EXIT_SUCCESS;
        setsid();
        if (fork() != 0)
            return EXIT_SUCCESS;
    }
    /* Make note of our actual process ID */
    if (pf)
    {
        fprintf(pf, "%d", (int)getpid());
        fclose(pf);
    }

    srs = srs_new();
    while ((secret = fgets(secretbuf, sizeof(secretbuf), sf)))
    {
        secret = strtok(secret, "\r\n");
        if (secret)
            srs_add_secret(srs, secret);
    }
    fclose(sf);

    srs_set_alwaysrewrite(srs, always_rewrite);
    srs_set_separator(srs, separator);
    if (hashlength)
        srs_set_hashlength(srs, hashlength);
    if (hashmin)
        srs_set_hashmin(srs, hashmin);

    for (sc = 0; sc < socket_count; ++sc)
    {
        fds[sc].fd = sockets[sc];
        fds[sc].events = POLLIN;
    }
    while (TRUE)
    {
        int conn;
        FILE *fp_read, *fp_write;
        char linebuf[1024], *line;
        char keybuf[1024], *key;

        if (poll(fds, socket_count, 1000) < 0)
        {
            if (errno == EINTR)
                continue;
            if (daemonize)
                syslog(LOG_MAIL | LOG_ERR, "Poll failure: %s", strerror(errno));
            else
                fprintf(stderr, "%s: Poll failure: %s\n", self,
                        strerror(errno));
            return EXIT_FAILURE;
        }
        for (sc = 0; sc < socket_count; ++sc)
        {
            if (fds[sc].revents)
            {
                conn = accept(fds[sc].fd, NULL, NULL);
                if (conn < 0)
                    continue;
                if (fork() == 0)
                {
                    int i;
                    /* close listen sockets so that we don't stop the main
                     * daemon process from restarting */
                    for (i = 0; i < socket_count; ++i)
                        close(sockets[i]);
                    /* create separate input/output streams */
                    fp_read = fdopen(conn, "r");
                    if (fp_read == NULL)
                        return EXIT_FAILURE;
                    fp_write = fdopen(dup(conn), "w");
                    if (fp_write == NULL)
                        return EXIT_FAILURE;
                    errno = 0;
                    alarm(timeout);
                    if (errno != 0)
                        return EXIT_FAILURE;
                    while ((line = fgets(linebuf, sizeof(linebuf), fp_read)))
                    {
                        char *token;
                        alarm(0);
                        if (strlen(line) >= sizeof(linebuf) - 1)
                        {
                            fprintf(fp_write, "500 Invalid request\n");
                            fflush(fp_write);
                            return EXIT_FAILURE;
                        }
                        token = strtok(line, " \r\n");
                        if (token == NULL || strcmp(token, "get") != 0)
                        {
                            fprintf(fp_write, "500 Invalid request\n");
                            fflush(fp_write);
                            return EXIT_FAILURE;
                        }
                        token = strtok(NULL, "\r\n");
                        if (!token)
                        {
                            fprintf(fp_write, "500 Invalid request\n");
                            fflush(fp_write);
                            return EXIT_FAILURE;
                        }
                        key = url_decode(keybuf, sizeof(keybuf), token);
                        if (!key)
                        {
                            fprintf(fp_write, "500 Invalid request\n");
                            fflush(fp_write);
                            return EXIT_FAILURE;
                        }
                        handler[sc](srs, fp_write, key, domain, excludes);
                        fflush(fp_write);
                        errno = 0;
                        alarm(timeout);
                        if (errno != 0)
                            return EXIT_FAILURE;
                    }
                    fclose(fp_write);
                    fclose(fp_read);
                    return EXIT_SUCCESS;
                }
                close(conn);
            }
        }
        waitpid(-1, NULL, WNOHANG);
    }
    return EXIT_SUCCESS;
}