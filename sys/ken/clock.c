#
#include "../param.h"
#include "../systm.h"
#include "../user.h"
#include "../proc.h"

#define	UMODE	0170000
#define	SCHMAG	10

/*
 * clock is called straight from
 * the real time clock interrupt.
 *
 * Functions:
 *	reprime clock
 *	copy *switches to display
 *	implement callouts
 *	maintain user/system times
 *	maintain date
 *	profile
 *	tout wakeup (sys sleep)
 *	lightning bolt wakeup (every 4 sec)
 *	alarm clock signals
 *	jab the scheduler
 */
clock(dev, sp, r1, nps, r0, pc, ps)
{
	register struct callo *p1, *p2;
	register struct proc *pp;

	/*
	 * restart clock
	 */

	*lks = 0115;

	/*
	 * display register
	 */

	display();	/// display see m40.s:66: does nothing

	/*
	 * callouts
	 * if none, just return
	 * else update first non-zero time
	 */

	if(callout[0].c_func == 0)
		goto out;
	p2 = &callout[0];
	while(p2->c_time<=0 && p2->c_func!=0)
		p2++;
	p2->c_time--;

	/*
	 * if ps is high, just return
	 */

	if((ps&0340) != 0)
		goto out;

	/*
	 * callout
	 */

	spl5();
	if(callout[0].c_time <= 0) {
		p1 = &callout[0];
		while(p1->c_func != 0 && p1->c_time <= 0) {	/// For each callout that has been timed up and has action
			(*p1->c_func)(p1->c_arg);
			p1++;
		}
		p2 = &callout[0];
		while(p2->c_func = p1->c_func) {	/// Delete the timers that have been tiggered
			p2->c_time = p1->c_time;
			p2->c_arg = p1->c_arg;
			p1++;
			p2++;
		}
	}

	/*
	 * lightning bolt time-out
	 * and time of day
	 */

out:
	if((ps&UMODE) == UMODE) {
		u.u_utime++;	/// Inc user level time
		if(u.u_prof[3])
			incupc(pc, u.u_prof);	/// incups see m40.s:70
	} else
		u.u_stime++;	/// Inc kernel/sys level time
	pp = u.u_procp;
	if(++pp->p_cpu == 0)	/// p_cpu is the time consumed by the current proc. If it overflows, make it biggest
		pp->p_cpu--;
	if(++lbolt >= HZ) {		/// tick number (between 0 and HZ)
		if((ps&0340) != 0)
			return;
		lbolt =- HZ;
		if(++time[1] == 0)	/// sec
			++time[0];
		spl1();
		if(time[1]==tout[1] && time[0]==tout[0])	/// Wake up proc who sleep because of sys call sleep
			wakeup(tout);	/// wakeup see slp.c:71
		if((time[1]&03) == 0) {
			runrun++;
			wakeup(&lbolt);
		}
		for(pp = &proc[0]; pp < &proc[NPROC]; pp++)	/// Recalculate p_time and p_cpu every 60 ticks
		if (pp->p_stat) {
			if(pp->p_time != 127)	/// How long it has been since it is in core or swapped out. This is reset when it is created, swapped in, or swapped out
				pp->p_time++;
			if((pp->p_cpu & 0377) > SCHMAG)	/// Decrease all proc's CPU time so that if a proc is not running,
											/// the cpu time of this proc will decrease, making its priority high
				pp->p_cpu =- SCHMAG; 
			else
				pp->p_cpu = 0;
			if(pp->p_pri > PUSER)	/// If it is a normal user proc
				setpri(pp);	/// setpri see slp.c:112
		}
		if(runin!=0) {
			runin = 0;
			wakeup(&runin);
		}
		if((ps&UMODE) == UMODE) {
			u.u_ar0 = &r0;
			if(issig())
				psig();
			setpri(u.u_procp);
		}
	}
}

/*
 * timeout is called to arrange that
 * fun(arg) is called in tim/HZ seconds.
 * An entry is sorted into the callout
 * structure. The time in each structure
 * entry is the number of HZ's more
 * than the previous entry.
 * In this way, decrementing the
 * first entry has the effect of
 * updating all entries.
 */
timeout(fun, arg, tim)
{
	register struct callo *p1, *p2;
	register t;
	int s;

	t = tim;
	s = PS->integ;
	p1 = &callout[0];
	spl7();
	while(p1->c_func != 0 && p1->c_time <= t) {
		t =- p1->c_time;
		p1++;
	}
	p1->c_time =- t;
	p2 = p1;
	while(p2->c_func != 0)
		p2++;
	while(p2 >= p1) {
		(p2+1)->c_time = p2->c_time;
		(p2+1)->c_func = p2->c_func;
		(p2+1)->c_arg = p2->c_arg;
		p2--;
	}
	p1->c_time = t;
	p1->c_func = fun;
	p1->c_arg = arg;
	PS->integ = s;
}
