/*********************************************************************
 *
 * File: Mortgage.H
 *
 * This is the main header file for the Mortgage computation program.
 *
 *********************************************************************/
#define HEADER	"Mortgage parameter calculation program"
#define VERSION	" version 1.10\n\n"
#define SYNTAX	"Syntax: Mortgage [<sourcefile>] [-s<outfile>]\n\nwhere:\n\n"\
	"\t<sourcefile> contains a parameter list.  If <sourcefile> is not\n"\
	"\t  specified (or incomplete), manual input is required.\n"\
	"\t-s<outfile> - Outputs a payment schedule.  If <outfile> is not\n"\
	"\t  specified, schedule is output to the screen.\n\n"
#define SYNTAXERROR	"\nERROR! - Invalid Syntax\n"
#define NOSOURCE	"\nERROR! - Cannot open \"%s\".\n"
#define BADSOURCE "\nERROR! - Invalid syntax in source file \"%s\".\n"
#define BADOUTPUT  "\nWARNING - Cannot open \"%s\" for output.\n"
#define BADPARMS   "\nERROR! - Incorrect number of parameters.\n"
#define TOOMANYPARMS	"\nWARNING - All parameters have been specified.  Recomputing payment.\n"
#define	TOOFEWPARMS	"\nERROR! - Can't compute with more than one parameter missing.\n"
#define RYANDEBT  "\nWARNING! - The $%.2f payment is less than the $%.2f interest.\n"\
		  "The mortgage will never be paid off.\n"
#define COULDNOTSOLVE	"\nERROR! - Could not solve problem.\n"

#define SCHEDULETOP	"\nDate                Interest    Principal  Remaining Debt  Total Interest\n"\
				"------------------------------------------------------------------------------\n"
