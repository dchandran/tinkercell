/*
	nonlinear optimization using simplex method
	developed by Nelder and Mead
	
	simplex method minimizes an object function
	this method requires no derivative information
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <values.h>
#include "opt.h"

#define	Debug		0
#define	Static		static

#define SKIPTIME	100	/* print interval for debugging */

static int	nvar;
static dbl	(*objective)(int n, double x[]);

static dbl	**simp, *fvalue;
static int	ih, is, il;
static dbl	*xcentroid;
static dbl	fmean, fvar;

static dbl	*xreflect,  *xcontract, *xexpand;
static dbl	freflect, fcontract, fexpand;
static dbl	al = 1, bt = 0.5, gm = 2;

static void fprint_simplex(FILE *fd)
{
	int	i;
	
	for (i=0; i<=nvar; i++) {
		vectorfprint(fd, nvar, simp[i]);
	}
}

static void fprint_points(FILE *fd)
{
	fprintf(fd, "----- xh xs and xl -----\n");
	vectorfprint(fd, nvar, simp[ih]);
	vectorfprint(fd, nvar, simp[is]);
	vectorfprint(fd, nvar, simp[il]);
	fprintf(fd, "----- xcentroid -----\n");
	vectorfprint(fd, nvar, xcentroid);
}

static void fprint_generated_points(FILE *fd)
{
	fprintf(fd, "----- xreflect xcontract xexpand -----\n");
	vectorfprint(fd, nvar, xreflect);
	vectorfprint(fd, nvar, xcontract);
	vectorfprint(fd, nvar, xexpand);
	fprintf(fd, "freflect %lf	fcontract %lf	fexpand %lf\n",
		freflect, fcontract, fexpand);
}

static void initialize()
{
	int	i;
	
	simp = alloc(dbl *, nvar+1);
	for (i=0; i<=nvar; i++) {
		simp[i] = alloc(dbl, nvar);
	}
	fvalue = alloc(dbl, nvar+1);
	
	xcentroid = alloc(dbl, nvar);
	
	xreflect  = alloc(dbl, nvar);
	xcontract = alloc(dbl, nvar);
	xexpand   = alloc(dbl, nvar);
}

static void initial_simplex(dbl *xinit, dbl length)
{
	int	i, j;
	dbl	a, d1, d2, *v;
	
	a = nvar + 1;
	d1 = (sqrt(a) + nvar - 1)/sqrt(2.00)/nvar;
	d2 = (sqrt(a) - 1)/sqrt(2.00)/nvar;
	
	v = simp[0];
	for (j=0; j<nvar; j++) v[j] = 0.00;
	for (i=1; i<=nvar; i++) {
		v = simp[i];
		for (j=0; j<nvar; j++) {
			v[j] = d2;
		}
		v[i-1] = d1;
	}
	
	for (i=0; i<=nvar; i++) {
		v = simp[i];
		scalarvector(nvar, v, length, v);
		vectoradd(nvar, v, xinit, v);
	}
}

static void search_simplex()
{
	int	i;
	
	if (fvalue[0] > fvalue[1]) {
		ih = 0;
		is = il = 1;
	} else {
		ih = 1;
		is = il = 0;
	}
	/* fprintf(stderr, "%d %d %d\n", ih, is, il); */
	
	for (i=2; i<=nvar; i++) {
		if (fvalue[i] > fvalue[ih]) {
			is = ih;
			ih = i;
		} else if (fvalue[i] > fvalue[is]) {
			is = i;
		} else if (fvalue[i] < fvalue[il]) {
			il = i;
		}
		/* fprintf(stderr, "%d %d %d\n", ih, is, il); */
	}
}

static void compute_xcentroid()
{
	int	i, j;
	dbl	*x;
	
	for (j=0; j<nvar; j++) xcentroid[j] = 0.00;
	for (i=0; i<=nvar; i++) {
		if (i == ih) continue;
		x = simp[i];
		for (j=0; j<nvar; j++) xcentroid[j] += x[j];
	}
	for (j=0; j<nvar; j++) xcentroid[j] /= nvar;
}

static void compute_fmean_fvar()
{
	int	i;
	dbl	d;
	
	fmean = 0.00;
	for (i=0; i<=nvar; i++) fmean += fvalue[i];
	fmean /= (nvar+1);
	
	fvar = 0.00;
	for (i=0; i<=nvar; i++) {
		d = fvalue[i] - fmean;
		fvar += d*d;
	}
	fvar /= (nvar+1);
}

static void reflection()
{
	int	j;
	dbl	*xh;
	
	xh = simp[ih];
	for (j=0; j<nvar; j++) {
		xreflect[j] = (1+al)*xcentroid[j] - al*xh[j];
	}
	freflect = (*objective)(nvar, xreflect);
}

static void contraction()
{
	int	j;
	dbl	*xh;
	
	xh = simp[ih];
	for (j=0; j<nvar; j++) {
		xcontract[j] = (1-bt)*xcentroid[j] + bt*xh[j];
	}
	fcontract = (*objective)(nvar, xcontract);
}

static void expansion()
{
	int	j;
	
	for (j=0; j<nvar; j++) {
		xexpand[j] = gm*xreflect[j] + (1-gm)*xcentroid[j];
	}
	fexpand = (*objective)(nvar, xexpand);
}

/*
	minimize function f(x) using
	Nelder and Mead's simplex method
	
	inputs: n --- the number of variables (dimension)
		f --- objective function
			dbl f(int n, dbl x[])
		xinit --- initial value
		length --- initial length of simplex
		timeout --- the maximum number of iterations
		eps --- small real number to test convergence
		
	outputs: xinit --- solution
		 *fopt --- optimal value
	return value: --- suceess, failure, or timeout
								*/

extern status NelderMeadSimplexMethod(
int	n,
dbl	(*f)(int n, double x[]),
dbl	*xinit,
dbl	length,
dbl	*fopt,
int	timeout,
dbl	eps)
{
	status	stat = failure;
	int	count, i;
	
	nvar = n;
	objective = f;
	
	initialize();
	initial_simplex(xinit, length);
	/* fprint_simplex(stderr); */
	for (i=0; i<=nvar; i++) {
		fvalue[i] = (*objective)(nvar, simp[i]);
	}
	/* vectorfprint(stderr, nvar+1, fvalue); */
	
	for (count=0; count<timeout; count++) {
		search_simplex();
		compute_xcentroid();
		/* fprint_points(stderr); */
		
		compute_fmean_fvar();
		/* fprintf(stderr, "fvar = %40.35f\n", fvar); */
		if (fvar <= eps) {
			stat = success;
			break;
		}
#if Debug
		if (count % SKIPTIME == 0) {
			fprintf(stderr, "k = %d   f = %lg\n", count, fvalue[il]);
			/* vectorfprint(stderr, nvar, xinit); */
		}
#endif
		reflection();
		if (freflect <= fvalue[is]) {
			if (freflect >= fvalue[il]) {
				vectorcopy(nvar, simp[ih], xreflect);
				fvalue[ih] = freflect;
			} else {
				expansion();
				if (fexpand < fvalue[il]) {
					vectorcopy(nvar, simp[ih], xexpand);
					fvalue[ih] = fexpand;
				} else {
					vectorcopy(nvar, simp[ih], xreflect);
					fvalue[ih] = freflect;
				}
			}
		} else {
			if (freflect < fvalue[ih]) {
				vectorcopy(nvar, simp[ih], xreflect);
				fvalue[ih] = freflect;
			}
			contraction();
			if (fcontract < fvalue[ih]) {
				vectorcopy(nvar, simp[ih], xcontract);
				fvalue[ih] = fcontract;
			} else {
				for (i=0; i<=nvar; i++) {
					if (i == il) continue;
					vectoradd(nvar, simp[i], simp[i], simp[il]);
					scalarvector(nvar, simp[i], 0.50, simp[i]);
					fvalue[i] = (*objective)(nvar, simp[i]);
				}
			}
		}
#if Debug
		/* fprintf(stderr, "%d : min = %lf\n", count, fvalue[il]); */
#endif
	}
	
	vectorcopy(nvar, xinit, simp[il]);
	*fopt = fvalue[il];
	
	return stat;
}

