////////////////////////////////////////////////////////////////////////////
//
// TODOS		By J. Flynn 5/23/98
//
// Converts a list of possibly wildcarded text files 
// from unix format to DOS format
//
////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <utime.h>
#if defined __APPLE__
#include <string.h>
#include <sys/stat.h>
#else
#include <sys\stat.h>
#endif

#define CR 0xd
#define LF 0xa
#define CZ 0x1a

/*
   TODOS - a Unix to DOS file converter
   by J. Flynn 01/03/98
*/

int main(int argc,char **argv)
{
	int i,ch,converted=0;
	int nconversions=0;
	struct stat sfs;
	struct utimbuf utbf;
	char msg[64];
	char filename[256];
	FILE *st,*ost;

	if (argc<2)
	{
		printf("Usage: TODOS filespec...\n");
		exit(1);
	}

	for (i=1;i<argc;i++) // note: uses "globbing" or auto wildcard expansion
	{
		// open next input file
		strcpy(filename,argv[i]);
		st = fopen(filename,"rb");

		if (st) // successful
		{
			// check for some things that could go wrong
			if (stat(filename,&sfs))
			{
				printf("WARNING: does not exist: %s, skipped\n",filename);
				fclose(st);
				continue;
			}
			if (S_ISDIR(sfs.st_mode))
			{
				printf("WARNING: attempt to convert directory: %s, skipped\n",filename);
				fclose(st);
				continue;
			}
			
			// save the datestamp on the input
			utbf.modtime = sfs.st_mtime;
			utbf.actime	= sfs.st_atime;

			// open temporary output file
			ost = fopen("TEMP.TMP","wb");
			if (ost) // successful
			{
				int ch,last=-1;

				// process the file char by char
				converted=0;
				while ((ch=getc(st)) != EOF)
				{
					// replace LF not preceeded by CR with a CRLF
					if (ch==LF && last!=CR)
					{
						putc(CR,ost);
						putc(LF,ost);
						converted=1;
					}
					else if (ch!=CZ)
						putc(ch,ost);	// else just copy char not ^Z
					last = ch;			// remember previous char
				}
				if (last!=LF)			// insure CRLF termination
				{
					putc(CR,ost);
					putc(LF,ost);
				}
				fclose(ost);
			}
			else
			{
				printf("FATAL: Cant open TEMP.TMP for output\n");
				fclose(st);
				exit(2);
			}

			fclose(st);

			if (converted)
			{
				if (rename("TEMP.TMP",filename))
				{
					sprintf(msg,"Error renaming to %s",filename);
					perror(msg);
					continue;
				}

				nconversions++;

				if (utime(filename,&utbf))
				{
					sprintf(msg,"Error setting timestamp of %s",filename);
					perror(msg);
					continue;
				}
				printf("%s converted from Unix format to DOS\n",filename);
			}
			else
				printf("%s did not need conversion\n",filename);
		}
		else
		{
			printf("FATAL: Cant open %s for input\n",filename);
			exit(2);
		}
	}
	remove("TEMP.TMP"); // insure this is gone

	printf("%d files converted from UNIX to DOS\n",nconversions);
	exit(0);
}
