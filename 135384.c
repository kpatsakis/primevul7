int vt_ioctl(struct tty_struct *tty,
	     unsigned int cmd, unsigned long arg)
{
	struct vc_data *vc = tty->driver_data;
	void __user *up = (void __user *)arg;
	int i, perm;
	int ret;

	/*
	 * To have permissions to do most of the vt ioctls, we either have
	 * to be the owner of the tty, or have CAP_SYS_TTY_CONFIG.
	 */
	perm = 0;
	if (current->signal->tty == tty || capable(CAP_SYS_TTY_CONFIG))
		perm = 1;

	ret = vt_k_ioctl(tty, cmd, arg, perm);
	if (ret != -ENOIOCTLCMD)
		return ret;

	ret = vt_io_ioctl(vc, cmd, up, perm);
	if (ret != -ENOIOCTLCMD)
		return ret;

	switch (cmd) {
	case TIOCLINUX:
		return tioclinux(tty, arg);
	case VT_SETMODE:
	{
		struct vt_mode tmp;

		if (!perm)
			return -EPERM;
		if (copy_from_user(&tmp, up, sizeof(struct vt_mode)))
			return -EFAULT;
		if (tmp.mode != VT_AUTO && tmp.mode != VT_PROCESS)
			return -EINVAL;

		console_lock();
		vc->vt_mode = tmp;
		/* the frsig is ignored, so we set it to 0 */
		vc->vt_mode.frsig = 0;
		put_pid(vc->vt_pid);
		vc->vt_pid = get_pid(task_pid(current));
		/* no switch is required -- saw@shade.msu.ru */
		vc->vt_newvt = -1;
		console_unlock();
		break;
	}

	case VT_GETMODE:
	{
		struct vt_mode tmp;
		int rc;

		console_lock();
		memcpy(&tmp, &vc->vt_mode, sizeof(struct vt_mode));
		console_unlock();

		rc = copy_to_user(up, &tmp, sizeof(struct vt_mode));
		if (rc)
			return -EFAULT;
		break;
	}

	/*
	 * Returns global vt state. Note that VT 0 is always open, since
	 * it's an alias for the current VT, and people can't use it here.
	 * We cannot return state for more than 16 VTs, since v_state is short.
	 */
	case VT_GETSTATE:
	{
		struct vt_stat __user *vtstat = up;
		unsigned short state, mask;

		if (put_user(fg_console + 1, &vtstat->v_active))
			return -EFAULT;

		state = 1;	/* /dev/tty0 is always open */
		console_lock(); /* required by vt_in_use() */
		for (i = 0, mask = 2; i < MAX_NR_CONSOLES && mask;
				++i, mask <<= 1)
			if (vt_in_use(i))
				state |= mask;
		console_unlock();
		return put_user(state, &vtstat->v_state);
	}

	/*
	 * Returns the first available (non-opened) console.
	 */
	case VT_OPENQRY:
		console_lock(); /* required by vt_in_use() */
		for (i = 0; i < MAX_NR_CONSOLES; ++i)
			if (!vt_in_use(i))
				break;
		console_unlock();
		i = i < MAX_NR_CONSOLES ? (i+1) : -1;
		return put_user(i, (int __user *)arg);

	/*
	 * ioctl(fd, VT_ACTIVATE, num) will cause us to switch to vt # num,
	 * with num >= 1 (switches to vt 0, our console, are not allowed, just
	 * to preserve sanity).
	 */
	case VT_ACTIVATE:
		if (!perm)
			return -EPERM;
		if (arg == 0 || arg > MAX_NR_CONSOLES)
			return -ENXIO;

		arg--;
		console_lock();
		ret = vc_allocate(arg);
		console_unlock();
		if (ret)
			return ret;
		set_console(arg);
		break;

	case VT_SETACTIVATE:
		if (!perm)
			return -EPERM;

		return vt_setactivate(up);

	/*
	 * wait until the specified VT has been activated
	 */
	case VT_WAITACTIVE:
		if (!perm)
			return -EPERM;
		if (arg == 0 || arg > MAX_NR_CONSOLES)
			return -ENXIO;
		return vt_waitactive(arg);

	/*
	 * If a vt is under process control, the kernel will not switch to it
	 * immediately, but postpone the operation until the process calls this
	 * ioctl, allowing the switch to complete.
	 *
	 * According to the X sources this is the behavior:
	 *	0:	pending switch-from not OK
	 *	1:	pending switch-from OK
	 *	2:	completed switch-to OK
	 */
	case VT_RELDISP:
		if (!perm)
			return -EPERM;

		console_lock();
		ret = vt_reldisp(vc, arg);
		console_unlock();

		return ret;


	 /*
	  * Disallocate memory associated to VT (but leave VT1)
	  */
	 case VT_DISALLOCATE:
		if (arg > MAX_NR_CONSOLES)
			return -ENXIO;

		if (arg == 0)
			vt_disallocate_all();
		else
			return vt_disallocate(--arg);
		break;

	case VT_RESIZE:
	{
		struct vt_sizes __user *vtsizes = up;
		struct vc_data *vc;
		ushort ll,cc;

		if (!perm)
			return -EPERM;
		if (get_user(ll, &vtsizes->v_rows) ||
		    get_user(cc, &vtsizes->v_cols))
			return -EFAULT;

		console_lock();
		for (i = 0; i < MAX_NR_CONSOLES; i++) {
			vc = vc_cons[i].d;

			if (vc) {
				vc->vc_resize_user = 1;
				/* FIXME: review v tty lock */
				vc_resize(vc_cons[i].d, cc, ll);
			}
		}
		console_unlock();
		break;
	}

	case VT_RESIZEX:
		if (!perm)
			return -EPERM;

		return vt_resizex(vc, up);

	case VT_LOCKSWITCH:
		if (!capable(CAP_SYS_TTY_CONFIG))
			return -EPERM;
		vt_dont_switch = true;
		break;
	case VT_UNLOCKSWITCH:
		if (!capable(CAP_SYS_TTY_CONFIG))
			return -EPERM;
		vt_dont_switch = false;
		break;
	case VT_GETHIFONTMASK:
		return put_user(vc->vc_hi_font_mask,
					(unsigned short __user *)arg);
	case VT_WAITEVENT:
		return vt_event_wait_ioctl((struct vt_event __user *)arg);
	default:
		return -ENOIOCTLCMD;
	}

	return 0;
}