#
/*
 */

/*
 * This table is the switch used to transfer
 * to the appropriate routine for processing a system call.
 * Each row contains the number of arguments expected
 * and a pointer to the routine.
 */
int	sysent[]
{
	0, &nullsys,			/*  0 = indir */	/// nullsys see trap.c:217
	0, &rexit,			/*  1 = exit */		/// rexit see sys1.c:214
	0, &fork,			/*  2 = fork */		/// fork see sys1.c:337
	2, &read,			/*  3 = read */		/// read see sys2.c:12
	2, &write,			/*  4 = write */	/// write see sys2.c:20
	2, &open,			/*  5 = open */		/// open see sys2.c:65
	0, &close,			/*  6 = close */	/// close see sys2.c:143
	0, &wait,			/*  7 = wait */		/// wait see sys1.c:284
	2, &creat,			/*  8 = creat */	/// creat see sys2.c:80
	2, &link,			/*  9 = link */		/// link see sys2.c:204
	1, &unlink,			/* 10 = unlink */	/// unlink see sys4.c:103
	2, &exec,			/* 11 = exec */		/// exec see sys1.c:24
	1, &chdir,			/* 12 = chdir */	/// chdir see sys4.c:130
	0, &gtime,			/* 13 = time */		/// gtime see sys4.c:22
	3, &mknod,			/* 14 = mknod */	/// mknod see sys2.c:246
	2, &chmod,			/* 15 = chmod */	/// chmod see sys4.c:151
	2, &chown,			/* 16 = chown */	/// chown see sys4.c:165
	1, &sbreak,			/* 17 = break */	/// sbreak see sys1.c:369
	2, &stat,			/* 18 = stat */		/// stat see sys3.c:31
	2, &seek,			/* 19 = seek */		/// seek see sys2.c:157
	0, &getpid,			/* 20 = getpid */	/// getpid see sys4.c:76
	3, &smount,			/* 21 = mount */	/// smount see sys3.c:86
	1, &sumount,			/* 22 = umount */	/// sumount see sys3.c:143
	0, &setuid,			/* 23 = setuid */	/// setuid see sys4.c:39
	0, &getuid,			/* 24 = getuid */	/// getuid see sys4.c:51
	0, &stime,			/* 25 = stime */	/// stime see sys4.c:29
	3, &ptrace,			/* 26 = ptrace */	/// ptrace see sig.c:256
	0, &nosys,			/* 27 = x */		/// nosys see trap.c:209
	1, &fstat,			/* 28 = fstat */	/// fstat see sys3.c:18
	0, &nosys,			/* 29 = x */
	1, &nullsys,			/* 30 = smdate; inoperative */
	1, &stty,			/* 31 = stty */		/// stty see tty.c:94
	1, &gtty,			/* 32 = gtty */		/// gtty see tty.c:75
	0, &nosys,			/* 33 = x */
	0, &nice,			/* 34 = nice */		/// nice see sys4.c:87
	0, &sslep,			/* 35 = sleep */	/// sslep see sys2.c:273
	0, &sync,			/* 36 = sync */		/// sync see sys4.c:81
	1, &kill,			/* 37 = kill */		/// kill see sys4.c:217
	0, &getswit,			/* 38 = switch */	/// getswit see sys4.c:16
	0, &nosys,			/* 39 = x */
	0, &nosys,			/* 40 = x */
	0, &dup,			/* 41 = dup */		/// dup see sys3.c:70
	0, &pipe,			/* 42 = pipe */		/// pipe see pipe.c:28
	1, &times,			/* 43 = times */	/// times see sys4.c:242. Syscall to get user level time
	4, &profil,			/* 44 = prof */		/// profil see sys4.c:252
	0, &nosys,			/* 45 = tiu */
	0, &setgid,			/* 46 = setgid */	/// setgid see sys4.c:58
	0, &getgid,			/* 47 = getgid */	/// getgid see sys4.c:69
	2, &ssig,			/* 48 = sig */		/// ssig see sys4.c:202
	0, &nosys,			/* 49 = x */
	0, &nosys,			/* 50 = x */
	0, &nosys,			/* 51 = x */
	0, &nosys,			/* 52 = x */
	0, &nosys,			/* 53 = x */
	0, &nosys,			/* 54 = x */
	0, &nosys,			/* 55 = x */
	0, &nosys,			/* 56 = x */
	0, &nosys,			/* 57 = x */
	0, &nosys,			/* 58 = x */
	0, &nosys,			/* 59 = x */
	0, &nosys,			/* 60 = x */
	0, &nosys,			/* 61 = x */
	0, &nosys,			/* 62 = x */
	0, &nosys			/* 63 = x */
};
