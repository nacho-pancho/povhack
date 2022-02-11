#include "vt100.h"

// these are the attribute flags defined in tty.h
// 
//#define VT100_ATTR_COLOR_MASK 0x3ffffff
//#define VT100_ATTR_COLOR_TYPE 0x3000000
//#define VT100_COLOR_OFF 0
//#define VT100_COLOR_16  1
//#define VT100_COLOR_256 2
//#define VT100_COLOR_RGB 3
//#define VT100_ATTR_BOLD         0x04000000
//#define VT100_ATTR_DIM          0x08000000
//#define VT100_ATTR_ITALIC       0x10000000
//#define VT100_ATTR_UNDERLINE    0x20000000
//#define VT100_ATTR_STRIKE       0x40000000
//#define VT100_ATTR_INVERSE      0x0400000000000000
//#define VT100_ATTR_BLINK        0x0800000000000000
//#define VT100_ATTR_CJK          0x8000000000000000

char vtattr_to_color(uint64_t attr) {
  return (attr & VT100_ATTR_BOLD) ?  (attr & 0x07) : (attr & 0x07) + 8;
}

//------------------------------------------------------------

static void hruler(FILE* out) {
  fputc(' ',out);
  for (int j = 0; j < NH_COLS; j++) {
    fputc(j % 10 ? '-' : ('0'+j/10),out);
  }
  fputc('\n',out);
}

//------------------------------------------------------------

static void vruler(int i, FILE* out) {
  fputc(i % 10 ? '|' : ('0'+i/10),out);
}
//------------------------------------------------------------

void tty_dump(tty vt, FILE* out) {
  attrchar* ac = vt->scr;
  printf("chars:\n");
  hruler(out);
  for (int i = 0; i < NH_ROWS; i++) {
    vruler(i,out);
    for (int j = 0; j < NH_COLS; j++, ac++) {
      fputc(ac->ch,out);
    }
    vruler(i,out);
    fputc('\n',out);
  }
  fprintf(out,"color:\n");
  ac = vt->scr;
  hruler(out);
  for (int i = 0; i < NH_ROWS; i++) {
    vruler(i,out);
    for (int j = 0; j < NH_COLS; j++, ac++) {
      int color = vtattr_to_color(ac->attr);
      fputc(color > 9? 'A'+ color: '0' + color,out);
    }
    vruler(i,out);
    fputc('\n',out);
  }  
  hruler(out);
  fprintf(out,"cursor at i=%d j=%d\n",vt->cy,vt->cx);
}


//------------------------------------------------------------

void tty_to_frame(tty vt, frame_t* frame) {
  //
  // copy top message and status bars
  //
  for (int j = 0; j < NH_COLS; j++) {
    frame->message[j] = vtget(vt,0,j).ch;
    frame->status1[j] = vtget(vt,22,j).ch;
    frame->status2[j] = vtget(vt,23,j).ch;
  }
  frame->message[NH_COLS] = 0;
  frame->status1[NH_COLS] = 0;
  frame->status2[NH_COLS] = 0;
  //
  // translate vt attrchars to glyphs
  // we only overwrite character and code
  // (we check that vt character and frame character are the same
  // on a given position)
  for (int i = 0; i < NH_ROWS; i++) {
    for (int j = 0; j < NH_COLS; j++) {
      attrchar ac = vtget(vt,i,j);
      glyph_t* gl = &frget(frame,i,j);
      gl->ascii = ac.ch;
      gl->color = vtattr_to_color(ac.attr);
    }
  }  
}

