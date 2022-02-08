#include <stdio.h>
#include <tty.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	const char *ifname = NULL, *ofname = NULL, *tfname = "tileset.pov";
	FILE *inf = NULL, *outf = NULL;

	if (argc < 3) {
		fprintf(stderr,"usage: %s [-t tileset.pov] <frame.tty> <frame.pov>\n",argv[0]);
		exit(1);
	}
	if ((argv[1][0] == '-') && (argv[1][1] == 't') ) {
		tfname = argv[2];
		argv += 2;
		argc -= 2;
	}
	if (argc < 3) {
		fprintf(stderr,"usage: %s [-t tileset.pov] <frame.tty> <frame.pov>\n",argv[0]);
		exit(1);
	}
	ifname = argv[1];
	ofname = argv[2];
	printf("FRAME2POV\n");
	printf("=======\n");
	printf("tiles : %s\n",tfname);
	printf("input : %s\n",ifname);
	printf("output: %s\n",ofname);
	printf("=======\n");

	inf = fopen(ifname,"r");
	//
	// read input, the whole of it
        //
	char chars[21*80]; 	
	char attrs[21*80];
	memset(chars,32,21*80);
	memset(attrs,0,21*80);
	int i,c;
	for (i = 0; (i < 21) && !feof(inf); ++i) {
	  for (int j = 0; !feof(inf) && ((c = fgetc(inf))!= '\n'); ++j ) {
	    putchar(c);
	    chars[i*80+j] = c;
	  }  
	  putchar(c);
	}

	printf("read %d char lines\n",i);
	for (i = 0; (i < 21) && !feof(inf); ++i) {
	  for (int j = 0; !feof(inf) && ((c = fgetc(inf))!= '\n'); ++j ) {
	    attrs[i*80+j] = c;
	    putchar(c);
	  }  
	  putchar(c);
	}
	printf("read %d attr lines\n",i);
	char dir = '.';
	if (!feof(inf)) {
	  dir = fgetc(inf);
	  printf("read direction %c\n",dir);
	}
	fclose(inf);
	//
	// infer direction of character
	// 
	int dirx, diry;
	int angle = 0;
	switch (dir) {
	  case 'h':
	    dirx = -1;
	    diry =  0; 
	    angle = -90;
	    break;
	  case 'j':
	    dirx =  0;
	    diry =  1; 
	    angle = 180;
	    break;
	  case 'k':
	    dirx =  0;
	    diry = -1; 
	    angle = 0;
	    break;
	  case 'l':
	    dirx =  1;
	    diry =  0; 
	    angle = 90;
	    break;
	  case 'y':
	    dirx = -1;
	    diry = -1; 
	    angle = -45;
	    break;
	  case 'u':
	    dirx =  1;
	    diry = -1; 
	    angle = 45;
	    break;
	  case 'b':
	    dirx = -1;
	    diry =  1; 
	    angle = -135;
	    break;
	  case 'n':
	    dirx =  1;
	    diry =  1; 
	    angle = 135;
	    break;
	  case '.':
	    dirx =  1;
	    diry = -1; 
	    break;
	}
	const int h = 2;
	outf = fopen(ofname,"w");
	// ugly...
	fprintf(outf,"#declare Angle=%d;\n",angle);
        fprintf(outf,"#include \"%s\"\n",tfname);

	int cx = 0, cy = 0;
	for (i = 0; (i < 21) ; ++i) {
	  for (int j = 0; j < 80; ++j ) {
	    const int chr = chars[i*80+j];
	    const char a  = attrs[i*80+j];
	    int col,bol;
	    if (a == 'X') {
	      bol = 1;
	      col = 7;
	      cx = j; cy = i+1;
	    } else if (a >= 'a') {
	      bol = 1;
	      col = a - 'a';
	    } else {
	      bol = 0;
	      col = a - 'A';
	    }
	    if (chr == '.') {
	      // a door between walls is not floor and should not be lit
	      // unless some neighbor is lit as well
	      int n = i > 0  ? (chars[(i-1)*80+j]=='.') && (attrs[(i-1)*80+j]>='a') : 0;
	      int s = i < 20 ? (chars[(i+1)*80+j]=='.') && (attrs[(i+1)*80+j]>='a') : 0;
	      int w = j > 0  ? (chars[i*80+j-1]=='.') && (attrs[i*80+j-1]>='a') : 0;
	      int e = j < 79 ? (chars[i*80+j+1]=='.') && (attrs[i*80+j+1]>='a') : 0;
	      if (!(n || s || w || e)) {
	        bol = 0;
	      } 
	    }
	    fprintf(outf,"object { Tiles[%d][%d][%d] translate %d*x+%d*z }\n",chr,col,bol,j,(20-i));
	  }  
	}
	fprintf(outf,"light_source { GlobalLight translate %d*x+%d*z }\n",cx,21-cy);
	fprintf(outf,"light_source { LocalLight  translate %f*x+%f*z }\n",cx+0.1*dirx,21-cy-0.1*diry);
	fprintf(outf,"camera {\n");
	fprintf(outf,"  perspective\n");
	fprintf(outf,"  right (1920/1080)*x\n");
	fprintf(outf,"  sky y\n");
	fprintf(outf,"  location <%d,%d,%d> \n",cx-h*dirx,2*h,21-cy+h*diry);
	fprintf(outf,"  look_at  <%d,0.5,%d>\n",cx,21-cy);
	fprintf(outf,"}\n");

	fclose(outf);
}

