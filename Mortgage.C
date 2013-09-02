/*************************************************************************
 *
 * File: Mortgage.C
 *
 * This is the Mortgage Calculation Program.  It takes as input any three
 * of the four mortgage parameters; Mortgage, Interest, Number of Payments,
 * and Monthly Payment, and calculates the fourth.  It also (optionally)
 * computes the payment schedule.
 *
 * This file compiles for DOS and OS/2 using Borland C++ v3.1 and
 * IBM VisualAge C++ v3.0 respectively.
 *
 *************************************************************************/
#include    "Mortgage.H"
#include    <stdio.H>
#include    <string.h>
#include    <stdlib.H>
#include    <math.H>
#include    <time.H>

int main(int argc, char *argv[])
{
    char    Prompt[][32]={"Mortgage Amount: $", "Annual Interest: ", "Number of Months: ",\
		"Monthly Payment: $", "Start Month : ", "Start Year: "};
    double   TempVal, Value[]={0,0,0,0,0,0};
    double   MonthYield, MonthYieldP1;
    int	    Spec[]={0,0,0,0,0,0};
    char    Key[][32]={"MORTGAGE", "INTEREST", "MONTHS", "PAYMENT", "STARTMONTH", "STARTYEAR"};
    char    *SourceFile=NULL, *Temp, *OutFile=NULL;
    int	    Schedule=0, ArgNum, Temp2;
    time_t  Time;
    FILE    *Source, *Output;
    char    Buffer[128];
    char    *TimeBuf;
    const char  MonthName[12][12]={"January", "February", "March", "April", "May", "June",\
		 "July", "August", "September", "October", "November", "December"};

    /* Output header */
    printf(HEADER);
    printf(VERSION);

    /* Determine current time */
    time(&Time);    /* Somehow convert this to month and year */
    TimeBuf=ctime(&Time);

    Value[5]=atoi(&TimeBuf[20]);
    for(ArgNum=0;ArgNum<11 && strnicmp(&TimeBuf[4], MonthName[ArgNum], 3);ArgNum++);
    Value[4]=ArgNum;

    /* Parse command line arguments */
    for(ArgNum=1;ArgNum<argc;ArgNum++)
    {
        if(argv[ArgNum][0]!='-')
            SourceFile=argv[ArgNum];
        else
        {
            switch(argv[ArgNum][1]) {
            case 's':
            case 'S':
                Schedule=1;
                OutFile=&argv[ArgNum][2];
                break;
            default:
                Schedule=2;
                ArgNum=argc;
                break;
            }
        }
    }
    if(Schedule>1)
    {
        printf(SYNTAXERROR);
        printf(SYNTAX);
        return Schedule;
    }
    /* Get any parameters available from a source file */
    if(SourceFile)
    {
        if(!(Source=fopen(SourceFile, "r")))
        {
            printf(NOSOURCE, SourceFile);
            return 2;
        }
        /* Input lines */
        while(fgets(Buffer, sizeof(Buffer), Source)>0 && Schedule<=1)
        {
            for(ArgNum=0, Temp2=-1;ArgNum<sizeof(Value)/sizeof(Value[0]) && Temp2<0;ArgNum++)
                if(!(strnicmp(Buffer, Key[ArgNum], strlen(Key[ArgNum]))))
                    { Temp2=ArgNum; Temp=strchr(Buffer, '=');}
            if(Temp2>=0 && Temp && !Spec[Temp2])
            {
                while(*(++Temp)==' ' || *Temp=='$'); /* Ignore these characters */

                if(Temp2!=4) Value[Temp2]=atof(Temp);
                else
                {
                    for(ArgNum=0;ArgNum<12 && !Spec[Temp2];ArgNum++)
                        if(!(strnicmp(Temp, MonthName[ArgNum], 3)))
                            {Value[Temp2]=ArgNum; Spec[Temp2]=1;}

                    if(!Spec[Temp2]) Schedule=2;
                }
                Spec[Temp2]=1;
            } else if(Buffer[0]!='\n' && Buffer[0]!=';') Schedule=2;
        }
        fclose(Source);
    }
    if(Schedule>1)
    {
        printf(BADSOURCE, SourceFile);
        return Schedule;
    }
    /* Determine how many of the first four parameters were specified */
    for(ArgNum=0, Temp2=0;ArgNum<4;ArgNum++) if(Spec[ArgNum]) Temp2++;

    /* This had better be three or less */
    for(ArgNum=0;ArgNum<4 && Temp2!=3;ArgNum++)
    {
        if(!Spec[ArgNum])
        {
            /* Ask for parameter */
            printf(Prompt[ArgNum]);
            fflush(stdout);     /* For bug in IBM C++ compiler */
            gets(Buffer);
            if(Buffer[0]!='\0' && Buffer[0]!='\n' && Buffer[0]!='\r')
            {
                Value[ArgNum]=atof(Buffer);
                Spec[ArgNum]=1;
                Temp2++;
            }
        }
    }
    if(Temp2==4)
    {
        /* Indicate overspecified, default to no payment specified */
        printf(TOOMANYPARMS);
        Spec[3]=0;
        Temp2--;
    } else if(Temp2<3)
    {
        printf(TOOFEWPARMS);
        return 3;
    }
    /* Ask for date if not provided, and if needed */
    if(Schedule && (!Spec[4] || ! Spec[5]))
    {
        TempVal=-1;
        printf(Prompt[4]);
        fflush(stdout);     /* For bug in IBM C++ compiler */
        gets(Buffer);
        for(ArgNum=0;ArgNum<12 && TempVal<0;ArgNum++)
            if(!(strnicmp(Buffer, MonthName[ArgNum], 3))) {Value[4]=ArgNum; TempVal=0;}

        if(TempVal>=0)
        {
            printf(Prompt[5]);
            fflush(stdout);     /* For bug in IBM C++ compiler */
            gets(Buffer);
            if(Buffer[0]) Value[5]=atof(Buffer);
        }
    }
    /* Now we determine which parameter we must calculate */
    for(ArgNum=0;Spec[ArgNum];ArgNum++);

    /* Precalculate monthly yield */
    MonthYield=Value[1]/100/12;
    MonthYieldP1=MonthYield+1;

    switch(ArgNum) {
    case 0:
        /* The mortgage amount has not been specified */
        Value[0]=Value[3]*(pow(MonthYieldP1,Value[2])-1)/(pow(MonthYieldP1,Value[2])*MonthYield);
        break;
    case 1:
        /* The interest rate has not been specified */
        /* Don't know how to (easily) solve this analytically, so we solve recursively. */
        /* M/P=(a^n-1)/(a^n*(a-1)), or P/M*((a^n-1)/a^n)=a-1 */
        /* We start by saying a=P/M+1, solve for a, then recompute left side */
        /* Continue until a varies by less than a certain threshold */
        ArgNum=5000;    /* Set computational limit */
        for(MonthYieldP1=Value[3]/Value[0]+1, MonthYield=-1;fabs(MonthYieldP1-MonthYield)>0.000001 && ArgNum;ArgNum--)
        {
            MonthYield=MonthYieldP1;
            MonthYieldP1=1+Value[3]/Value[0]*(pow(MonthYieldP1,Value[2])-1)/pow(MonthYieldP1,Value[2]);
        }
        if(ArgNum)
        {
            /* Convert into an interest percentage */
            MonthYield=MonthYieldP1-1;
            Value[1]=MonthYield*1200;
        } else {printf(COULDNOTSOLVE); return 4;}
        break;
    case 2:
        /* The number of payments has not been specified */
        /* Verify that the loan can ever be paid */
        if(Value[0]*MonthYield>=Value[3])
        {
            printf(RYANDEBT, Value[3], Value[0]*MonthYield);
            if(Output) fprintf(Output, RYANDEBT, Value[3], Value[0]*MonthYield);
            Schedule=2; /* Don't print schedule */
        } else
        {
            Value[2]=log(Value[3]/(Value[3]-Value[0]*MonthYield))/log(MonthYieldP1);
            /* Round up */
            if(modf(Value[2], &TempVal)) Value[2]=TempVal+1;
        }
        break;
    default:
        /* The value of each month's payment has not been specified */
        Value[3]=Value[0]*(pow(MonthYieldP1,Value[2]))*MonthYield/(pow(MonthYieldP1,Value[2])-1);
        break;
    }
    /* Now we output results */
    if(Schedule)
    {
        if(OutFile && OutFile[0])
        {
            if(!(Output=fopen(OutFile, "w")))
            printf(BADOUTPUT, OutFile);
        } else Output=stdout;
    }
    printf("\n");
    if(OutFile && Output)
    {
        fprintf(Output, HEADER);
        fprintf(Output, VERSION);
    }
    for(ArgNum=0;ArgNum<4 && Schedule<=1;ArgNum++)
    {
        printf("%s%.2f",Prompt[ArgNum], Value[ArgNum]);
        if(OutFile && Output) fprintf(Output, "%s%.2f",Prompt[ArgNum], Value[ArgNum]);

        /* Indicate which parameter was computed */
        if(Spec[ArgNum])
        {
            printf("\n");
            if(Output && OutFile) fprintf(Output,"\n");
        } else
        {
            printf(" (computed)\n");
            if(Output && OutFile) fprintf(Output, " (computed)\n");
        }
    }
    /* Was a payment schedule requested? */
    if(Schedule==1)
    {
        fprintf(Output, SCHEDULETOP);

        MonthYieldP1=0; /* Now hold total interest paid */

        for(ArgNum=0;ArgNum<Value[2];ArgNum++)
        {
            /* We pre-increment the date, because the first payment */
            /* isn't due until next month */
            if(++Value[4]>11) {Value[4]=0; Value[5]++;}
            fprintf(Output, "%-10s %4.0f   ",MonthName[(int)Value[4]], Value[5]);

            TempVal=Value[0]*MonthYield;
            fprintf(Output, "%10.2f   ", TempVal);
            Value[0]+=TempVal;
            MonthYieldP1+=TempVal;
            if(Value[0]<Value[3])
            {
                fprintf(Output, "%10.2f    ", Value[0]-TempVal);
            }
            else
            {
                fprintf(Output, "%10.2f    ", Value[3]-TempVal);
            }
            Value[0]-=Value[3];
            if(Value[0]<0) Value[0]=0;
            fprintf(Output, "%10.2f    ", Value[0]);
            fprintf(Output, "%12.2f\n", MonthYieldP1);
        }
    }
    if(Output && OutFile) fclose(Output);
    return 0;
}