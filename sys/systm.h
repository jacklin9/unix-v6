/*
 * Random set of variables
 * used by more than one
 * routine.
 */
char	canonb[CANBSIZ];	/* buffer for erase and kill (#@) */
int	coremap[CMAPSIZ];	/* space for core allocation */
int	swapmap[SMAPSIZ];	/* space for swap allocation */
int	*rootdir;		/* pointer to inode of root directory */
int	cputype;		/* type of cpu =40, 45, or 70 */	/// See m40.s:833
int	execnt;			/* number of processes in exec */
int	lbolt;			/* time of day in 60th not in time */
int	time[2];		/* time in sec from 1970 */
int	tout[2];		/* time of day of next sleep */
/*
 * The callout structure is for
 * a routine arranging
 * to be called by the clock interrupt
 * (clock.c) with a specified argument,
 * in a specified amount of time.
 * Used, for example, to time tab
 * delays on teletypes.
 */
struct	callo
{
	int	c_time;		/* incremental time */
	int	c_arg;		/* argument to routine */
	int	(*c_func)();	/* routine */
} callout[NCALL];
/*
 * Mount structure.
 * One allocated on every mount.
 * Used to find the super block.
 */
struct	mount
{
	int	m_dev;		/* device mounted */
	int	*m_bufp;	/* pointer to superblock */
	int	*m_inodp;	/* pointer to mounted-on inode */
} mount[NMOUNT];
int	mpid;			/* generic for unique process id's */
char	runin;			/* scheduling flag */	/// Wait to check if there is swapped out proc deserve swapping out an in-core proc to free up core
char	runout;			/* scheduling flag */	/// Wait there is swapped out proc to be swapped in
char	runrun;			/* scheduling flag */	/// Wait for proc reschedule
char	curpri;			/* more scheduling */
int	maxmem;			/* actual max memory per process */
int	*lks;			/* pointer to clock device */
int	rootdev;		/* dev of root see conf.c */
int	swapdev;		/* dev of swap see conf.c */ 
int	swplo;			/* block number of swap space */
int	nswap;			/* size of swap space */	/// Configured in mkconf.c
int	updlock;		/* lock for sync */
int	rablock;		/* block to be read ahead */
char	regloc[];		/* locs. of saved user registers (trap.c) */
