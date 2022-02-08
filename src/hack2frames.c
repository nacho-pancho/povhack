#include <stdio.h>
#include <tty.h>
#include <stdlib.h>

void dump_frame(tty vt,int cursor_x, int cursor_y);

void write_frame(const char* prefix, tty vt, int idx, int cursor_x, int cursor_y, int dir);
int capture_time(int c, int* time);
int detect_begin_frame(int c);
void fix_cursor(tty vt, int* cx, int *cy);
char get_direction(int cx, int cy);

#define get(t,i,j) ((t)->scr[(i)*80+(j)] )

int main(int argc, char *argv[]) {
	const char *ifname = NULL, *ofprefix = NULL;
	int cursor_x = 0, cursor_y = 0;
	FILE *inf = NULL;

	if (argc < 3) {
		fprintf(stderr,"usage: %s <vt100dump> <frame-prefix>\n",argv[0]);
		exit(1);
	}
	ifname = argv[1];
	ofprefix = argv[2];
	printf("POVHACK\n");
	printf("=======\n");
	printf("input : %s\n",ifname);
	printf("output: %s_nnnnn.tty \n",ofprefix);
	printf("=======\n");
	//
	// prepare TTY info
	// 
	tty vt = tty_init(80,24,1); // 80x21, resizable
	inf = fopen(ifname,"r");
	//
	// detect first frame: last thing to be printed is status bar, ending with T:N
	// 
	int frame    = 0;
	int prevtime = 0;
	int time     = 0;
	tty_reset(vt);
	int save_frame = 0;
	int capture_cursor = 1;
	int prev_dir = 'l';
	while (!feof(inf)) {
          int c = fgetc(inf);
	  // look for T:nnnn
	  prevtime = time;
	  if (capture_time(c,&time) && (time > prevtime)) 
	    save_frame = 1;
	  // heuristic to detect beginning of frame [23,1H
	  if (capture_cursor && (c == '@')) {
	    cursor_x = vt->cx;
	    cursor_y = vt->cy;
	    printf("cursor at %d,%d\n",vt->cx, vt->cy);
	    capture_cursor = 0;
	  }
	  // just prior to this, cursor is placed on top of avatar 
	  if (detect_begin_frame(c)) {
	    // time advanced: redraw screen
            if (save_frame) { 
	      printf("saving frame %d time %d\n",frame, time);
	      fix_cursor(vt,&cursor_x,&cursor_y); // some times it is slightly off
	      char dir = get_direction(cursor_x,cursor_y);
	      if (dir == '.') {
	        dir = prev_dir;
	      } else {
	        prev_dir = dir;
	      }
	      printf("direction: %c\n",dir);
	      //dump_frame(vt,cursor_x,cursor_y);
	      write_frame(ofprefix,vt,time,cursor_x,cursor_y,dir);
	      frame++;
	      save_frame = 0;
	    }
	    capture_cursor = 1;
	  }
	  //  update terminal, at last
	  tty_write(vt,(char*)&c,1);
	}
	printf("processed %d frames\n",frame);
	fclose(inf);
	tty_free(vt);
	exit(0);
}

int detect_begin_frame(int c) {
  // detect the escape sequence ESC[23;1H
  const char needle[] = {'\033','[','2','3',';','1','H'};
  static char buffer[7];
  // update buffer
  for (int i = 0; i < 6; ++i) {
    buffer[i] = buffer[i+1];
  }
  buffer[6] = c;
  // check for first coincidence
  if (c == 'H') {
    for (int i = 0; i < 7; ++i) {
      if (buffer[i] != needle[i])  { 
        return 0; 
      }
    }
    return 1; 
  } else if (c == 'J') {
    // E[2J  is 'clear screen'
    if ((buffer[3] == '\033') && (buffer[4] == '[') && (buffer[5] == '2')) {
      return 1;
    }  
  } 
  return 0;
}

int capture_time(int c, int* time) {
  static int c2, c1, c0; // past two chars, c[-2],c[-1]
  static int counter = 0;
  static int in_time = 0;
  c2 = c1; c1 = c0; c0 = c;
  // detect string: 'T:n...'
  if (!in_time) {
    if ((c2 == 'T') && (c1 == ':') && (c0 >= '0') && (c0 <= '9')) {
      // begin computing time
      in_time = 1;
      counter = c0 - '0';
      return 0;
    } else {
      return 0;
    }
  } else  {// continue computing time
    if ((c0 >= '0') && (c0 <= '9')) { // new digit
      counter = 10*counter + (c0-'0');
      return 0;
    } else { // no new digits: update time and reset
      in_time = 0; // no longer computing time
      *time = counter;
      return 1;
    }
  }
}

void dump_frame(tty vt,int cx, int cy) {
	const int m = vt->sy > 22 ? 22 : vt->sy;
	const int n = vt->sx;
	attrchar* scr = vt->scr;
	printf("+");
	for (int j = 0; j < n; ++j) {
		putchar(j == cx ? '*' : ( (j+1) % 10 ? '-' : '+') );
	}
	putchar('\n');
	for (int i = 1; i < m; ++i) {
	  putchar(i == cy? '*': (i % 10 ? '|' : '+'));
	  for (int j = 0; j < n; ++j) {
	    const int k = i*n+j;
	    const int  c = scr[k].ch; 
	    //const unsigned long  a = scr[k].attr;
      	    // extract color and inverse mode
            //const int inv = (a & VT100_ATTR_INVERSE) > 0  ? 1 : 0;
	    //const int col = a & 0x0f;
	    //const int att1 = (a >>24) & 0xff;
	    //const int att2 = (a >>56) & 0xff;
	    //fprintf(outf,"%3d %lx %2d %d %d %d\n",c,a,col,att1,att2,inv);
	    putchar(c);
	  }
	  putchar('\n');
	}
  printf("\n");
}

void write_frame(const char* prefix, tty vt, int idx, int cx, int cy, int dir) {
	char tmp[512];
	FILE* outf;
	snprintf(tmp,512,"%s_%05d.tty",prefix,idx);
	outf = fopen(tmp,"w");
	attrchar* scr = vt->scr;
	const int m = vt->sy > 22 ? 22 : vt->sy;
	const int n = vt->sx;
	for (int i = 1; i < m; ++i) {
	  for (int j = 0; j < n; ++j) {
	    const int k = i*n+j;
	    const int  c = scr[k].ch; 
	    fputc(c,outf);
	  }
	  fputc('\n',outf);
	}
	for (int i = 1; i < m; ++i) {
	  for (int j = 0; j < n; ++j) {
	    const int k = i*n+j;
	    const unsigned long  a = scr[k].attr;
            const int bold = (a & VT100_ATTR_BOLD) > 0  ? 1 : 0;
	    const int col  = a & 0x0f;
            // our own encoding of color + bold using printable chars
	    const int c = bold ? 'A' + col : 'a' + col; 
	    if ((i == cy) && (j == cx)) {
	      fputc('X',outf); // cursor position
	    } else {
	      fputc(c,outf);
	    }
	  }
	  fputc('\n',outf);
	}
	// last char is direction
	fputc(dir,outf);
	fputc('\n',outf);
	fclose(outf);
}

void fix_cursor(tty vt, int* pcx, int* pcy) {
	// 
	// ensure that the symbol below the cursor is a '@'
	//
        attrchar ac = get(vt,*pcy,*pcx);
	if (ac.ch != '@') {
	  printf("fixing cursor!\n");
	  for (int dx = -1; dx <= 1; ++dx) {
	    for (int dy = -1; dy <= 1; ++dy) {
              ac = get(vt,*pcy+dy,*pcx+dx); // south?
              if (ac.ch == '@') {
                *pcx += dx;
		*pcy += dy;
		return;
	      }
	    }
	  }
	  printf("cursor not found!!\n");
	}
}

char get_direction(int cx, int cy) {
  static int cx0 = -1, cy0 = -1;
  int res = -1;
  char dirmap[3][3] = { {'y','k','u'}, {'h','.','l'} ,{'b','j','n'} };
  if (cx0 < 0) {
    res = '.';
  } else {
    printf("(%d,%d) -> (%d,%d)\n",cy0,cx0,cy,cx);
    int dy = cy > cy0? 1 : (cy < cy0 ? -1 : 0);
    int dx = cx > cx0? 1 : (cx < cx0 ? -1 : 0);	    
    res = dirmap[1+dy][1+dx];
  }
  cx0 = cx;
  cy0 = cy;
  return res;
}
