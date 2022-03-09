////////////////////////////////////////////////////////////////////////////
//
// MAKTRLMP		By J. Flynn 5/23/98
//
// Converts a TRANMAP.DAT file created by BOOM to a custom transluceny lump
// suitable for inclusion in a PWAD by deleting its header data.
//
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#if !defined __APPLE__
#include <malloc.h>
#endif
#include <string.h>

//
// AddExt
//
// Force a specified extension to filename, 
// adding it if none present, replacing it if one is
//
// str:   string to force extension on, must provide enough space to add ext
// ext:   extension, including period, to add
//
void AddExt(char *str,char *ext)
{
	char *p,*q;

	if (!str || !*str)
		return;

	p = strrchr(str,'\\');		// last backslash
	q = strrchr(str,'.');		  // last period
	if (p && q && q<p)			  // if is a backslash, and the . is before it
		strcat(str,ext);		    // add an extension, since there is none
	else if (q)					      // else if is a period, replace the extension
	{							
		*q='\0';
		strcat(str,ext);
	}
	else if (!q)				      // else if no period, default the extension
		strcat(str,ext);
}

//
// main
//
// Delete the header from a specified tranlucency file copied from
// TRANMAP.DAT, creating a .LMP file by the same name that can be
// used as a custom translucency lump in a PWAD
//
int main(int argc,char **argv)
{
	FILE *st;
	char filename[256];
	char *p;

	if (argc!=2)
	{
		printf("Usage: MAKTRLMP file.ext\n");
	  exit(1);
	}

	strcpy(filename,argv[1]);		  // force a .DAT extension on input file
	AddExt(filename,".DAT");

	st=fopen(filename,"rb");
	if (st)
	{
#if !defined __APPLE__
		p = malloc(65536);				  // output is always exactly 64K in size
#else
		p = (char *)malloc(65536);				  // output is always exactly 64K in size
#endif
		if (p)
		{
			fseek(st,257,SEEK_SET);	  // skip past the header
			if (fread(p,256,256,st)!=256)				     // read the data
			{
				printf("File %s corrupt\n",filename);	 // if not enough, corrupt input
				fclose(st);
				exit(1);
			}
			fclose(st);
			AddExt(filename,".LMP");	// force .LMP extension on output file
			st = fopen(filename,"wb");
			if (st)
			{
				if (fwrite(p,256,256,st)!=256)         // write the data read
				{
					printf("Out of disk space\n");			 // if can't assume disk full
					fclose(st);
					exit(1);
				}
				fclose(st);
			}
			else
			{
				printf("Can't open %s for writing\n",filename);
				exit(1);
			}
		}
		else
		{
			printf("Insufficient memory\n");
			fclose(st);
			exit(1);
		}
	}
	else
	{
		printf("Can't open %s for reading\n",filename);
		exit(1);
	}
	printf("%s converted from tranlucency .DAT file to .LMP\n",filename);
	return 0;
}
