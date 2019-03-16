#define CHAR	01
#define BLOCK	02
#define INTR	04
#define EVEN	010
#define KL	020
#define ROOT	040

/// Generate 2 files: l.s and c.c
///
/// l.s is an assembly file that contains code of low level initialization and trap/interrupt entries
/// A sample is as follows.
/*
/ low core

br4 = 200
br5 = 240
br6 = 300
br7 = 340

. = 0^.
	br	1f
	4

/ trap vectors
	trap; br7+0.		/ bus error
	trap; br7+1.		/ illegal instruction
	trap; br7+2.		/ bpt-trace trap
	trap; br7+3.		/ iot trap
	trap; br7+4.		/ power fail
	trap; br7+5.		/ emulator trap
	trap; br7+6.		/ system entry

. = 40^.
.globl	start, dump
1:	jmp	start
	jmp	dump


. = 60^.
	klin; br4
	klou; br4

. = 100^.
	kwlp; br6
	kwlp; br6

. = 114^.
	trap; br7+7.		/ 11/70 parity

. = 220^.
	rkio; br5

. = 240^.
	trap; br7+7.		/ programmed interrupt
	trap; br7+8.		/ floating point
	trap; br7+9.		/ segmentation violation

//////////////////////////////////////////////////////
/		interface code to C
//////////////////////////////////////////////////////

.globl	call, trap

.globl	_klrint
klin:	jsr	r0,call; _klrint
.globl	_klxint
klou:	jsr	r0,call; _klxint

.globl	_clock
kwlp:	jsr	r0,call; _clock


.globl	_rkintr
rkio:	jsr	r0,call; _rkintr
*/
/// c.c is a device driver interface file. A sample is as follows.
/*
int	(*bdevsw[])()
{
	&nulldev,	&nulldev,	&rkstrategy, 	&rktab,	/ rk 	0 /
	&nodev,		&nodev,		&nodev,		0,	/ tm 	1 /
	0
};

int	(*cdevsw[])()
{
	&klopen,   &klclose,  &klread,   &klwrite,  &klsgtty,	/ console 	0 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ pc 	1 /
	&lpopen,   &lpclose,  &nodev,    &lpwrite,  &lpsgtty,	/ lp 	2 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ lv 	3 /
	&nulldev,  &nulldev,  &mmread,   &mmwrite,  &nodev,	/ mem 	4 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ gld 	5 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ tm 	6 /
	&djopen,   &djclose,  &djread,   &djwrite,  &djsgtty,	/ dj 	7 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ crd 	8 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ vp 	9 /
	&nulldev,  &nulldev,  &rkread,   &rkwrite,  &nodev,	/ rk 	10 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ ta 	11 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ dr 	12 /
	&yjopen,   &yjclose,  &yjread,   &nodev,    &nodev,	/ cybjd 	13 /
	&yjopen,   &yjclose,  &nodev,    &yjwrite,  &nodev,	/ cybju 	14 /
	&ycopen,   &ycclose,  &ycread,   &nodev,    &nodev,	/ cybcd 	15 /
	&ycopen,   &ycclose,  &nodev,    &ycwrite,  &nodev,	/ cybcu 	16 /
	&nodev,    &nodev,    &nodev,    &nodev,    &nodev,	/ crdb 	17 /
	&craopen,  &crclose,  &crread,   &nodev,    &crsgtty,	/ cr 	18 /
	&crbopen,  &crclose,  &crread,   &nodev,    &crsgtty,	/ crb 	19 /
	&syopen,   &nulldev,  &syread,   &sywrite,  &sysgtty,	/ tty 	20 /
	0
};

int	rootdev	{(0<<8)|8};
int	swapdev	{(0<<8)|0};
int	swplo	1452;	/ cannot be zero /
int	nswap	984;
*/

char	*btab[]
{
	"rk",
	"rp",
	"rf",
	"tm",
	"tc",
	"hs",
	"hp",
	"ht",
	0
};
char	*ctab[]
{
	"console",
	"pc",
	"lp",
	"dc",
	"dh",
	"dp",
	"dj",
	"dn",
	"mem",
	"rk",
	"rf",
	"rp",
	"tm",
	"hs",
	"hp",
	"ht",
	0
};
struct tab
{
	char	*name;
	int	count;		/// Configured device count. If count != 0, the device exist and code for it needs be generated.
					/// If count = 0, the device does not exist. Count can generally be changed by input().
					/// The difference between count < 0 and count > 0 is count < 0 cannot be changed by input() and 
					/// the opposite number is the real count while count > 0 can be changed
	int	address;	/// Trap vector addr
	int	key;	/// Device type(char or block), whether it needs interrupt
	char	*codea;	/// Trap vector content
	char	*codeb;	/// Implementation of function called in trap vector
	char	*codec;
	char	*coded;
	char	*codee;
} table[]
{
	"console",
	-1,	60,	CHAR+INTR+KL,
	"\tklin; br4\n\tklou; br4\n",
	".globl\t_klrint\nklin:\tjsr\tr0,call; _klrint\n",	/// Push r0 to stack, move address of next instruction (which stores addr of _klrint) to r0, and jump to call
	".globl\t_klxint\nklou:\tjsr\tr0,call; _klxint\n",
	"",
	"\t&klopen,   &klclose,  &klread,   &klwrite,  &klsgtty,",

	"mem",
	-1,	300,	CHAR,
	"",	/// Codea
	"",	/// Codeb
	"",	/// Codec
	"",	/// Coded
	"\t&nulldev,  &nulldev,  &mmread,   &mmwrite,  &nodev,",	/// Codee

	"pc",
	0,	70,	CHAR+INTR,
	"\tpcin; br4\n\tpcou; br4\n",
	".globl\t_pcrint\npcin:\tjsr\tr0,call; _pcrint\n",
	".globl\t_pcpint\npcou:\tjsr\tr0,call; _pcpint\n",
	"",
	"\t&pcopen,   &pcclose,  &pcread,   &pcwrite,  &nodev,",

	"clock",	/// Clock
	-2,	100,	INTR,
	"\tkwlp; br6\n",
	".globl\t_clock\n",
	"kwlp:\tjsr\tr0,call; _clock\n",	/// _clock see clock.c:26
	"",
	"",

	"parity",
	-1,	114,	INTR,
	"\ttrap; br7+7.\t\t/ 11/70 parity\n",
	"",
	"",
	"",
	"",

/*
 * 110 unused
 * 114 memory parity
 * 120 XY plotter
 * 124 DR11-B
 * 130 AD01
 * 134 AFC11
 * 140 AA11
 * 144 AA11
 * 150-174 unused
 */

	"lp",
	0,	200,	CHAR+INTR,
	"\tlpou; br4\n",
	"",
	".globl\t_lpint\nlpou:\tjsr\tr0,call; _lpint\n",
	"",
	"\t&lpopen,   &lpclose,  &nodev,    &lpwrite,  &nodev,",

	"rf",
	0,	204,	BLOCK+CHAR+INTR,
	"\trfio; br5\n",
	".globl\t_rfintr\n",
	"rfio:\tjsr\tr0,call; _rfintr\n",
	"\t&nulldev,\t&nulldev,\t&rfstrategy, \t&rftab,",
	"\t&nulldev,  &nulldev,  &rfread,   &rfwrite,  &nodev,",

	"hs",
	0,	204,	BLOCK+CHAR+INTR,
	"\thsio; br5\n",
	".globl\t_hsintr\n",
	"hsio:\tjsr\tr0,call; _hsintr\n",
	"\t&nulldev,\t&nulldev,\t&hsstrategy, \t&hstab,",
	"\t&nulldev,  &nulldev,  &hsread,   &hswrite,  &nodev,",

/*
 * 210 RC
 */

	"tc",		/// This is the 3rd dev
	0,	214,	BLOCK+INTR,
	"\ttcio; br6\n",
	".globl\t_tcintr\n",
	"tcio:\tjsr\tr0,call; _tcintr\n",
	"\t&nulldev,\t&tcclose,\t&tcstrategy, \t&tctab,",
	"",

	"rk",		/// This is the 1st dev: root dev
	0,	220,	BLOCK+CHAR+INTR,
	"\trkio; br5\n",
	".globl\t_rkintr\n",
	"rkio:\tjsr\tr0,call; _rkintr\n",	/// When interrupt happens,  PS and PC will be pushed to stack.
										/// r0 will also be pushed to stack. Then r0 is loaded with addr of 
										/// next instruction (which stores addr of _rkintr), and jump to call
										/// call see m40.s:33. _rkintr see rk.c:98
	"\t&nulldev,\t&nulldev,\t&rkstrategy, \t&rktab,",
	"\t&nulldev,  &nulldev,  &rkread,   &rkwrite,  &nodev,",

	"tm",		/// This is the 2nd dev: tape
	0,	224,	BLOCK+CHAR+INTR,
	"\ttmio; br5\n",
	".globl\t_tmintr\n",
	"tmio:\tjsr\tr0,call; _tmintr\n",	/// call see m40.s:33. _tmintr see tm.c:161
	"\t&tmopen,\t&tmclose,\t&tmstrategy, \t&tmtab,",
	"\t&tmopen,   &tmclose,  &tmread,   &tmwrite,  &nodev,",

	"ht",
	0,	224,	BLOCK+CHAR+INTR,
	"\thtio; br5\n",
	".globl\t_htintr\n",
	"htio:\tjsr\tr0,call; _htintr\n",
	"\t&htopen,\t&htclose,\t&htstrategy, \t&httab,",
	"\t&htopen,   &htclose,  &htread,   &htwrite,  &nodev,",

	"cr",
	0,	230,	CHAR+INTR,
	"\tcrin; br6\n",
	"",
	".globl\t_crint\ncrin:\tjsr\tr0,call; _crint\n",
	"",
	"\t&cropen,   &crclose,  &crread,   &nodev,    &nodev,",

/*
 * 234 UDC11
 */

	"rp",
	0,	254,	BLOCK+CHAR+INTR,
	"\trpio; br5\n",
	".globl\t_rpintr\n",
	"rpio:\tjsr\tr0,call; _rpintr\n",
	"\t&nulldev,\t&nulldev,\t&rpstrategy, \t&rptab,",
	"\t&nulldev,  &nulldev,  &rpread,   &rpwrite,  &nodev,",

	"hp",
	0,	254,	BLOCK+CHAR+INTR,
	"\thpio; br5\n",
	".globl\t_hpintr\n",
	"hpio:\tjsr\tr0,call; _hpintr\n",
	"\t&hpopen,\t&nulldev,\t&hpstrategy, \t&hptab,",
	"\t&hpopen,   &nulldev,  &hpread,   &hpwrite,  &nodev,",

/*
 * 260 TA11
 * 264-274 unused
 */

	"dc",
	0,	308,	CHAR+INTR,
	"\tdcin; br5+%d.\n\tdcou; br5+%d.\n",
	".globl\t_dcrint\ndcin:\tjsr\tr0,call; _dcrint\n",
	".globl\t_dcxint\ndcou:\tjsr\tr0,call; _dcxint\n",
	"",
	"\t&dcopen,   &dcclose,  &dcread,   &dcwrite,  &dcsgtty,",

	"kl",
	0,	308,	INTR+KL,
	"\tklin; br4+%d.\n\tklou; br4+%d.\n",
	"",
	"",
	"",
	"",

	"dp",
	0,	308,	CHAR+INTR,
	"\tdpin; br6+%d.\n\tdpou; br6+%d.\n",
	".globl\t_dprint\ndpin:\tjsr\tr0,call; _dprint\n",
	".globl\t_dpxint\ndpou:\tjsr\tr0,call; _dpxint\n",
	"",
	"\t&dpopen,   &dpclose,  &dpread,   &dpwrite,  &nodev,",

/*
 * DM11-A
 */

	"dn",
	0,	304,	CHAR+INTR,
	"\tdnou; br5+%d.\n",
	"",
	".globl\t_dnint\ndnou:\tjsr\tr0,call; _dnint\n",
	"",
	"\t&dnopen,   &dnclose,  &nodev,    &dnwrite,  &nodev,",

	"dhdm",
	0,	304,	INTR,
	"\tdmin; br4+%d.\n",
	"",
	".globl\t_dmint\ndmin:\tjsr\tr0,call; _dmint\n",
	"",
	"",

/*
 * DR11-A+
 * DR11-C+
 * PA611+
 * PA611+
 * DT11+
 * DX11+
 */

	"dl",
	0,	308,	INTR+KL,
	"\tklin; br4+%d.\n\tklou; br4+%d.\n",
	"",
	"",
	"",
	"",

/*
 * DJ11
 */

	"dh",
	0,	308,	CHAR+INTR+EVEN,
	"\tdhin; br5+%d.\n\tdhou; br5+%d.\n",
	".globl\t_dhrint\ndhin:\tjsr\tr0,call; _dhrint\n",
	".globl\t_dhxint\ndhou:\tjsr\tr0,call; _dhxint\n",
	"",
	"\t&dhopen,   &dhclose,  &dhread,   &dhwrite,  &dhsgtty,",

/*
 * GT40
 * LPS+
 * VT20
 */

	0
};

char	*stra[]		/// This is the start of the kernel code
{
	"/ low core",
	"",
	"br4 = 200",
	"br5 = 240",
	"br6 = 300",
	"br7 = 340",
	"",
	". = 0^.",
	"\tbr\t1f",
	"\t4",
	"",
	"/ trap vectors",	/// When an interrupt happens, the process will automatically stores processor status and return address
						/// to stack, and load instruction pointer and proc status register with the first and second word in the
						/// trap vector. Eg, when a bus error occurs, the addr of trap routine will be loaded to instruction reg,
						/// and br7 + 0 will be loaded to proc status reg
	"\ttrap; br7+0.\t\t/ bus error",	/// trap see m40.s:13
	"\ttrap; br7+1.\t\t/ illegal instruction",
	"\ttrap; br7+2.\t\t/ bpt-trace trap",
	"\ttrap; br7+3.\t\t/ iot trap",
	"\ttrap; br7+4.\t\t/ power fail",
	"\ttrap; br7+5.\t\t/ emulator trap",
	"\ttrap; br7+6.\t\t/ system entry",
	"",
	". = 40^.",
	".globl\tstart, dump",
	"1:\tjmp\tstart",	/// See m40.s:708
	"\tjmp\tdump",
	"",
	0,
};

char	*strb[]
{
	"",
	". = 240^.",
	"\ttrap; br7+7.\t\t/ programmed interrupt",
	"\ttrap; br7+8.\t\t/ floating point",
	"\ttrap; br7+9.\t\t/ segmentation violation",
	0
};

char	*strc[]
{
	"",
	"/ floating vectors",
	". = 300^.",
	0,
};

char	*strd[]
{
	"",
	"//////////////////////////////////////////////////////",
	"/\t\tinterface code to C",
	"//////////////////////////////////////////////////////",
	"",
	".globl\tcall, trap",
	0
};

char	*stre[]
{
	"/*",
	" */",
	"",
	"int\t(*bdevsw[])()",
	"{",
	0,
};

char	*strf[]
{
	"\t0",
	"};",
	"",
	"int\t(*cdevsw[])()",
	"{",
	0,
};

char	*strg[]
{
	"\t0",
	"};",
	"",
	"int\trootdev\t{(%d<<8)|0};",
	"int\tswapdev\t{(%d<<8)|0};",
	"int\tswplo\t4000;\t/* cannot be zero */",
	"int\tnswap\t872;",
	0,
};

int	fout;
int	root	-1;

main()
{
	register struct tab *p;
	register *q;
	int i, n, ev, nkl;
	int flagf, flagb;

	while(input());	/// Set device count according to input

/*
 * pass1 -- create interrupt vectors
 */
	nkl = 0;
	flagf = flagb = 1;
	fout = creat("l.s", 0666);
	puke(stra);	/// Print exception trap handler
	for(p=table; p->name; p++)
	if(p->count != 0 && p->key & INTR) {	/// If device needs be configured and supports interrupt
		if(p->address>240 && flagb) {	/// When addr > 240, dump strb once
			flagb = 0;
			puke(strb);
		}
		if(p->address >= 300) {
			if(flagf) {	/// When addr >= 300, dump strc once
				ev = 0;
				flagf = 0;
				puke(strc);
			}
			if(p->key & EVEN && ev & 07) {
				printf("\t.=.+4\n");
				ev =+ 4;
			}
			ev =+ p->address - 300;
		} else
			printf("\n. = %d^.\n", p->address);
		n = p->count;
		if(n < 0)
			n = -n;
		for(i=0; i<n; i++)
			if(p->key & KL) {
				printf(p->codea, nkl, nkl);
				nkl++;
			} else
			printf(p->codea, i, i);
	}
	if(flagb)
		puke(strb);
	puke(strd);
	for(p=table; p->name; p++)
	if(p->count != 0 && p->key & INTR)
		printf("\n%s%s", p->codeb, p->codec);
	flush();
	close(fout);

/*
 * pass 2 -- create configuration table
 */

	fout = creat("c.c", 0666);
	puke(stre);	/// Start of block dev conf
	for(i=0; q=btab[i]; i++) {
		for(p=table; p->name; p++)
		if(equal(q, p->name) &&
		   (p->key&BLOCK) && p->count) {
			printf("%s\t/* %s */\n", p->coded, q);
			if(p->key & ROOT)
				root = i;
			goto newb;
		}
		printf("\t&nodev,\t\t&nodev,\t\t&nodev,\t\t0,\t/* %s */\n", q);
	newb:;
	}
	puke(strf);	/// Start of char dev conf
	for(i=0; q=ctab[i]; i++) {
		for(p=table; p->name; p++)
		if(equal(q, p->name) &&
		   (p->key&CHAR) && p->count) {
			printf("%s\t/* %s */\n", p->codee, q);
			goto newc;
		}
		printf("\t&nodev,    &nodev,    &nodev,    &nodev,    &nodev,\t/* %s */\n", q);
	newc:;
	}
	puke(strg, root);
	flush();
	close(fout);
	if(root < 0)
		write(2, "no block device given\n", 22);
}

puke(s, a)
char **s;
{
	char *c;

	while(c = *s++) {
		printf(c, a);
		printf("\n");
	}
}

input()
{
	char line[100];
	register char *p;
	register struct tab *q;
	register n;

	p = line;
	while((n=getchar()) != '\n') {	/// Read one line
		if(n == 0)
			return(0);
		if(n == ' ' || n == '\t')
			continue;
		*p++ = n;
	}
	*p++ = 0;
	n = 0;
	/// line should be of format [n] devname
	p = line;
	while(*p>='0' && *p<='9') {
		n =* 10;
		n =+ *p++ - '0';
	}
	if(n == 0)
		n = 1;
	if(*p == 0)
		return(1);
	for(q=table; q->name; q++)	/// Find the device
	if(equal(q->name, p)) {
		if(root < 0 && (q->key&BLOCK)) {
			root = 0;	/// Root is set
			q->key =| ROOT;
		}
		if(q->count < 0) {	/// This device doesn't need config. It is fixed
			printf("%s: no more, no less\n", p);
			return(1);
		}
		q->count =+ n;
		if(q->address < 300 && q->count > 1) {	/// Devices whose trap no is less than 300 has at most 1
			q->count = 1;
			printf("%s: only one\n", p);
		}
		return(1);
	}
	if(equal(p, "done"))
		return(0);
	printf("%s: cannot find\n", p);
	return(1);
}

equal(a, b)
char *a, *b;
{

	while(*a++ == *b)
		if(*b++ == 0)
			return(1);
	return(0);
}

getchar()
{
	int c;

	c = 0;
	read(0, &c, 1);
	return(c);
}
