/* file: psobject.prolog.cpp
  Copyright (C) 2008 Ville-Petteri Makinen

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the
  Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  CITATION (if not provided by software website)
    Makinen V-P, software name, URL:http://www.iki.fi/~vpmakine/

  CONTACT (if not provided by citation)
    Ville-Petteri Makinen
 
    Folkhalsan Research Center
    Biomedicum Helsinki P.O.Box 63
    Haartmaninkatu 8 00014, Helsinki, Finland
    Tel: +358 9 191 25462
    Fax: +358 9 191 25452

    WWW:   http://www.iki.fi/~vpmakine
*/

#include "psobject.h"

using namespace std;

static void get_color(char*, int);

/*
 *
 */
void
PSObject::prolog() {
  int k;
  unsigned long n = 0;
  float font_size = atof(parameters["FontSize"].c_str());
  char rgb[3];

  if(output == NULL) return;
  if(font_size < 2.0) font_size = 2.0; 
  if(font_size > 50.0) font_size = 50.0; 
  font_size /= CM2DOT_ps;

  /* Fonts. */
  n += fprintf(output, "\n%% fonts\n");
  n += fprintf(output, "/HELV_FIXED_16 {/Helvetica findfont ");
  n += fprintf(output, "%.4f ", 16.0/CM2DOT_ps);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV_FIXED_12 {/Helvetica findfont ");
  n += fprintf(output, "%.4f ", 12.0/CM2DOT_ps);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV_FIXED_10 {/Helvetica findfont ");
  n += fprintf(output, "%.4f ", 10.0/CM2DOT_ps);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV_FIXED_8 {/Helvetica findfont ");
  n += fprintf(output, "%.4f ", 8.0/CM2DOT_ps);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV_HUGE {/Helvetica-Bold findfont ");
  n += fprintf(output, "%.4f ", 2*font_size);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV_LARGE {/Helvetica findfont ");
  n += fprintf(output, "%.4f ", 3*font_size/2);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV {/Helvetica findfont ");
  n += fprintf(output, "%.4f ", font_size);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV_BOLD {/Helvetica-Bold findfont ");
  n += fprintf(output, "%.4f ", font_size);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV_ITALIC {/Helvetica-Italic findfont ");
  n += fprintf(output, "%.4f ", font_size);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/HELV_SMALL {/Helvetica findfont ");
  n += fprintf(output, "%.4f ", 4*font_size/5);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/COUR_BOLD {/Courier-Bold findfont ");
  n += fprintf(output, "%.4f ", font_size);
  n += fprintf(output, "scalefont setfont} def\n");
  n += fprintf(output, "/COUR_BOLD_SMALL {/Courier-Bold findfont ");
  n += fprintf(output, "%.4f ", 4*font_size/5);
  n += fprintf(output, "scalefont setfont} def\n");

  /* Predefined colors. */
  n += fprintf(output, "\n%% predefined colors\n");
  n += fprintf(output, "/BLACK {0 setgray} def\n");
  n += fprintf(output, "/GRAY {0.6 setgray} def\n");
  n += fprintf(output, "/WHITE {1 setgray} def\n");
  n += fprintf(output, "/RED {1 0.2 0.05 setrgbcolor} def\n");
  n += fprintf(output, "/GREEN {0.2 1 0.1 setrgbcolor} def\n");
  n += fprintf(output, "/BLUE {0.1 0.4 1 setrgbcolor} def\n");
  n += fprintf(output, "/YELLOW {1 0.95 0.5 setrgbcolor} def\n");
  n += fprintf(output, "/ORANGE {1 0.6 0.2 setrgbcolor} def\n");
  n += fprintf(output, "/BIRCH {1 0.95 0.8 setrgbcolor} def\n");
  n += fprintf(output, "/SKY {0.0 0.35 0.65 setrgbcolor} def\n");

  /* String procedures. */
  n += fprintf(output, "\n%% string procedures\n");
  n += fprintf(output, "/showr {");
  n += fprintf(output, "dup stringwidth add -1 mul 0 rmoveto show} def\n");
  n += fprintf(output, "/showc {");
  n += fprintf(output, "dup stringwidth add -0.5 mul 0 rmoveto show} def\n");

  /* Shorthands. */
  n += fprintf(output, "\n%% shorthands\n"); 
  get_color(rgb, atoi(parameters["BackgroundColor"].c_str()));
  n += fprintf(output, "/BG {%.4f ", rgb[0]/99.0);
  n += fprintf(output, "%.4f %.4f ", rgb[1]/99.0, rgb[2]/99.0);
  n += fprintf(output, "setrgbcolor} def\n");
  get_color(rgb, atoi(parameters["ForegroundColor"].c_str()));
  n += fprintf(output, "/FG {newpath %.4f ", rgb[0]/99.0);
  n += fprintf(output, "%.4f %.4f ", rgb[1]/99.0, rgb[2]/99.0);
  n += fprintf(output, "setrgbcolor 0.04 mul setlinewidth} def\n");
  n += fprintf(output, "/CP {charpath} def\n");
  n += fprintf(output, "/CL {closepath} def\n");
  n += fprintf(output, "/F {fill} def\n");
  n += fprintf(output, "/GS {gsave} def\n");
  n += fprintf(output, "/GR {grestore} def\n");
  n += fprintf(output, "/L {lineto} def\n");
  n += fprintf(output, "/RL {rlineto} def\n");
  n += fprintf(output, "/M {moveto} def\n");
  n += fprintf(output, "/NP {newpath} def\n");
  n += fprintf(output, "/S {stroke} def\n");
  n += fprintf(output, "/SC {setrgbcolor} def\n");
  n += fprintf(output, "/SG {setgray} def\n");
  n += fprintf(output, "/SL {0.04 mul setlinewidth} def\n");
  n += fprintf(output, "/FF {/Helvetica findfont} def\n");
  n += fprintf(output, "/SF {scalefont setfont} def\n");
  get_color(rgb, atoi(parameters["BackgroundColor"].c_str()));
  n += fprintf(output, "/SH {dup gsave %.4f ", rgb[0]/99.0);
  n += fprintf(output, "%.4f %.4f ", rgb[1]/99.0, rgb[2]/99.0);
  n += fprintf(output, "setrgbcolor\n  true charpath ");
  n += fprintf(output, "stroke grestore show} def\n");
  n += fprintf(output, "/SHC {dup stringwidth add -0.5 mul 0 rmoveto\n");
  n += fprintf(output, "  dup gsave %.4f ", rgb[0]/99.0);
  n += fprintf(output, "%.4f %.4f setrgbcolor\n", rgb[1]/99.0, rgb[2]/99.0);
  n += fprintf(output, "  true charpath stroke grestore show} def\n");

  /* Graphics primitives. */
  n += fprintf(output, "\n%% shape primitives\n");
  n += fprintf(output, "/s.1 {\n"); /* circle */
  n += fprintf(output, "newpath 0 360 arc closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/s.2 {\n"); /* triangle with tip up */
  n += fprintf(output, "1.25 mul /rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp yp moveto\n");
  n += fprintf(output, "rp 0.0000 mul rp 0.8000 mul rmoveto\n");
  n += fprintf(output, "rp -0.8660 mul rp -1.5000 mul rlineto\n");
  n += fprintf(output, "rp 1.7321 mul rp -0.0000 mul rlineto\n");
  n += fprintf(output, "rp -0.8660 mul rp 1.5000 mul rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/s.3 {\n"); /* cross */
  n += fprintf(output, "1.2 mul /rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp yp moveto\n");
  n += fprintf(output, "rp 0.3536 mul rp 0.3536 mul rmoveto\n");
  n += fprintf(output, "rp -0.0947 mul rp 0.6124 mul rlineto\n");
  n += fprintf(output, "rp -0.5176 mul rp 0.0000 mul rlineto\n");
  n += fprintf(output, "rp -0.0947 mul rp -0.6124 mul rlineto\n");
  n += fprintf(output, "rp -0.6124 mul rp -0.0947 mul rlineto\n");
  n += fprintf(output, "rp -0.0000 mul rp -0.5176 mul rlineto\n");
  n += fprintf(output, "rp 0.6124 mul rp -0.0947 mul rlineto\n");
  n += fprintf(output, "rp 0.0947 mul rp -0.6124 mul rlineto\n");
  n += fprintf(output, "rp 0.5176 mul rp -0.0000 mul rlineto\n");
  n += fprintf(output, "rp 0.0947 mul rp 0.6124 mul rlineto\n");
  n += fprintf(output, "rp 0.6124 mul rp 0.0947 mul rlineto\n");
  n += fprintf(output, "rp 0.0000 mul rp 0.5176 mul rlineto\n");
  n += fprintf(output, "rp -0.6124 mul rp 0.0947 mul rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/s.4 {\n"); /* diamond */
  n += fprintf(output, "/rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp rp sub yp moveto\n");
  n += fprintf(output, "rp rp 1.16 mul rlineto rp rp -1.16 mul rlineto\n");
  n += fprintf(output, "rp neg rp -1.16 mul rlineto closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/s.5 {\n"); /* star */
  n += fprintf(output, "1.2 mul /rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp yp moveto\n");
  n += fprintf(output, "rp 0.0000 mul rp -0.6000 mul rmoveto\n");
  n += fprintf(output, "rp 0.5878 mul rp -0.2090 mul rlineto\n");
  n += fprintf(output, "rp -0.0172 mul rp 0.6236 mul rlineto\n");
  n += fprintf(output, "rp 0.3804 mul rp 0.4944 mul rlineto\n");
  n += fprintf(output, "rp -0.5984 mul rp 0.1764 mul rlineto\n");
  n += fprintf(output, "rp -0.3527 mul rp 0.5146 mul rlineto\n");
  n += fprintf(output, "rp -0.3527 mul rp -0.5146 mul rlineto\n");
  n += fprintf(output, "rp -0.5984 mul rp -0.1764 mul rlineto\n");
  n += fprintf(output, "rp 0.3804 mul rp -0.4944 mul rlineto\n");
  n += fprintf(output, "rp -0.0172 mul rp -0.6236 mul rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/s.6 {\n"); /* triangle with tip down */
  n += fprintf(output, "1.25 mul /rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp yp moveto\n");
  n += fprintf(output, "rp 0.0000 mul rp -0.8000 mul rmoveto\n");
  n += fprintf(output, "rp 0.8660 mul rp 1.5000 mul rlineto\n");
  n += fprintf(output, "rp -1.7321 mul rp 0.0000 mul rlineto\n");
  n += fprintf(output, "rp 0.8660 mul rp -1.5000 mul rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/s.7 {\n"); /* square */
  n += fprintf(output, "0.9 mul /rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp rp add yp rp add moveto\n");
  n += fprintf(output, "0 rp -2 mul rlineto rp -2 mul 0 rlineto ");
  n += fprintf(output, "0 rp 2 mul rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/s.8 {\n"); /* rotated cross */
  n += fprintf(output, "1.2 mul /rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp yp moveto\n");
  n += fprintf(output, "rp 0.0000 mul rp 0.5000 mul rmoveto\n");
  n += fprintf(output, "rp -0.5000 mul rp 0.3660 mul rlineto\n");
  n += fprintf(output, "rp -0.3660 mul rp -0.3660 mul rlineto\n");
  n += fprintf(output, "rp 0.3660 mul rp -0.5000 mul rlineto\n");
  n += fprintf(output, "rp -0.3660 mul rp -0.5000 mul rlineto\n");
  n += fprintf(output, "rp 0.3660 mul rp -0.3660 mul rlineto\n");
  n += fprintf(output, "rp 0.5000 mul rp 0.3660 mul rlineto\n");
  n += fprintf(output, "rp 0.5000 mul rp -0.3660 mul rlineto\n");
  n += fprintf(output, "rp 0.3660 mul rp 0.3660 mul rlineto\n");
  n += fprintf(output, "rp -0.3660 mul rp 0.5000 mul rlineto\n");
  n += fprintf(output, "rp 0.3660 mul rp 0.5000 mul rlineto\n");
  n += fprintf(output, "rp -0.3660 mul rp 0.3660 mul rlineto\n");
  n += fprintf(output, "rp -0.5000 mul rp -0.3660 mul rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  /* Alternative primitives. */
  n += fprintf(output, "/square {\n");
  n += fprintf(output, "/rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp rp add yp rp add moveto\n");
  n += fprintf(output, "0 rp -2 mul rlineto rp -2 mul 0 rlineto ");
  n += fprintf(output, "0 rp 2 mul rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/rect {\n");
  n += fprintf(output, "/hp exch def /wp exch def ");
  n += fprintf(output, "/yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp wp add yp hp add moveto\n");
  n += fprintf(output, "0 hp -2 mul rlineto wp -2 mul 0 rlineto ");
  n += fprintf(output, "0 hp 2 mul rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/circle {\n");
  n += fprintf(output, "newpath 0 360 arc closepath\n");
  n += fprintf(output, "} def\n");

  n += fprintf(output, "/diamond {\n");
  n += fprintf(output, "1.15 mul /rp exch def /yp exch def /xp exch def\n");
  n += fprintf(output, "newpath xp rp sub yp moveto\n");
  n += fprintf(output, "rp rp rlineto rp rp neg rlineto ");
  n += fprintf(output, "rp neg rp neg rlineto\n");
  n += fprintf(output, "closepath\n");
  n += fprintf(output, "} def\n");

  /* Generic texture (digits). */
  n += fprintf(output, "\n%% textures\n");
  n += fprintf(output, "/texture {\n");
  n += fprintf(output, "/i exch def /r exch def ");
  n += fprintf(output, "/y exch def /x exch def\n");
  n += fprintf(output, "x y r 2.5 div sub moveto i 4 string cvs showc\n");
  n += fprintf(output, "} def\n");

  /* Textures. */
  n += fprintf(output, "/texture.1 {\n");
  n += fprintf(output, "/i exch def /r exch def ");
  n += fprintf(output, "/y exch def /x exch def\n");
  n += fprintf(output, "gsave x y translate 45 i mul rotate\n");
  n += fprintf(output, "2 r mul r moveto -4 r mul 0 rlineto stroke\n");
  n += fprintf(output, "2 r mul r 2 div moveto -4 r mul 0 rlineto stroke\n");
  n += fprintf(output, "2 r mul 0 moveto -4 r mul 0 rlineto stroke\n");
  n += fprintf(output, "2 r mul r -2 div moveto -4 r mul 0 rlineto stroke\n");
  n += fprintf(output, "2 r mul r neg moveto -4 r mul 0 rlineto stroke ");
  n += fprintf(output, "grestore\n} def\n");
  
  n += fprintf(output, "/texture.2 {\n");
  n += fprintf(output, "/i exch def /r exch def ");
  n += fprintf(output, "/y exch def /x exch def\n");
  n += fprintf(output, "gsave x y translate 90 i mul rotate ");
  n += fprintf(output, "r r r square fill\n");
  n += fprintf(output, "grestore\n} def\n");

  n += fprintf(output, "/texture.3 {\n");
  n += fprintf(output, "/i exch def /r exch def ");
  n += fprintf(output, "/y exch def /x exch def\n");
  n += fprintf(output, "gsave x y translate 90 i mul 45 add rotate\n");
  n += fprintf(output, "0 r r circle fill ");
  n += fprintf(output, "grestore\n} def\n");

  n += fprintf(output, "/texture.4 {\n");
  n += fprintf(output, "/i exch def /r exch def ");
  n += fprintf(output, "/y exch def /x exch def\n");
  n += fprintf(output, "gsave x y translate 90 i mul rotate ");
  n += fprintf(output, "0 r 2 r mul r rect fill ");
  n += fprintf(output, "grestore\n} def\n");

  n += fprintf(output, "/texture.5 {\n");
  n += fprintf(output, "/i exch def /r exch def ");
  n += fprintf(output, "/y exch def /x exch def\n");
  n += fprintf(output, "gsave x y translate 45 i mul rotate\n");
  n += fprintf(output, "r r r square fill r neg r neg r square fill ");
  n += fprintf(output, "grestore\n} def\n");

  n += fprintf(output, "/texture.6 {\n");
  n += fprintf(output, "/i exch def /r exch def ");
  n += fprintf(output, "/y exch def /x exch def\n");
  n += fprintf(output, "gsave x y translate 90 i mul rotate\n");
  n += fprintf(output, "2 {0 r 2 r mul r rect fill 90 rotate} repeat ");
  n += fprintf(output, "grestore\n} def\n");

  /* Configured patterns. */
  n += fprintf(output, "\n%% patterns of texture\n");
  n += fprintf(output, "/p.1 {pop pop pop} def\n");
  for(k = 2; k < (N_PATTERNS - 1); k++) {
    if((k/10 > 0) && (k/10 < 7) &&
       (k%10 > 0) && (k%10 < 5)) {
      n += fprintf(output, "/p.%d ", k);
      n += fprintf(output, "{%d texture.%d} def\n", k%10, k/10);
    }
    else {
      n += fprintf(output, "/p.%d ", k);
      n += fprintf(output, "{%d texture} def\n", k);
    }
  }
  n += fprintf(output, "/p.%d {", (N_PATTERNS - 1));
  n += fprintf(output, "square fill} def\n");

  /* Sircangle. */
  n += fprintf(output, "\n%% sircangle\n");
  n += fprintf(output, "/sircangle {\n");
  n += fprintf(output, "/r exch def /y exch def /x exch def\n");
  n += fprintf(output, "x r sub /x exch def\n");
  n += fprintf(output, "y r 2 div add /y exch def\n");
  n += fprintf(output, "x y r diamond ");
  n += fprintf(output, "gsave BG fill grestore stroke\n");
  n += fprintf(output, "x r add /x exch def \n");
  n += fprintf(output, "y r sub /y exch def\n");
  n += fprintf(output, "x y r square ");
  n += fprintf(output, "gsave BG fill grestore stroke\n");
  n += fprintf(output, "x y r 1.3 div sub moveto ");
  n += fprintf(output, "(male ) showc\n");
  n += fprintf(output, "x r add /x exch def \n");
  n += fprintf(output, "y r 1.1 div add /y exch def\n");
  n += fprintf(output, "x y 1.16 r mul circle ");
  n += fprintf(output, "gsave BG fill grestore stroke\n");
  n += fprintf(output, "y r 5 div sub /y exch def\n");
  n += fprintf(output, "x y moveto (female) showc\n");
  n += fprintf(output, "} def\n");
  
  /* RGB logo. */
  n += fprintf(output, "\n%% rgb\n");
  n += fprintf(output, "/rgb {\n");
  n += fprintf(output, "gsave /r exch def /y exch def /x exch def\n");
  n += fprintf(output, "GREEN x y r 2.7 div 0.65 r mul rect fill\n");
  n += fprintf(output, "x 2 2.7 div r mul sub /x exch def ");
  n += fprintf(output, "RED x y r 2.7 div 0.5 r mul rect fill\n");
  n += fprintf(output, "x 4 2.7 div r mul add /x exch def ");
  n += fprintf(output, "BLUE x y r 2.7 div 0.8 r mul rect fill\n");
  n += fprintf(output, "x 2 2.7 div r mul sub /x exch def\n");
  n += fprintf(output, "x y r 2.8 div sub moveto WHITE (RGB) showc ");
  n += fprintf(output, "grestore\n} def\n");

  /* Pencil. */
  n += fprintf(output, "\n%% pencil\n");
  n += fprintf(output, "/pencil {\n");
  n += fprintf(output, "gsave /r exch def /y exch def /x exch def\n");
  n += fprintf(output, "x y r 7 div sub translate 0 0 moveto -30 rotate\n");
  n += fprintf(output, "0.25 r mul 0.4 r mul moveto ");
  n += fprintf(output, "0.75 r mul -0.4 r mul rlineto\n");
  n += fprintf(output, "-0.75 r mul -0.4 r mul rlineto ");
  n += fprintf(output, "-1.25 r mul 0 rlineto\n");
  n += fprintf(output, "0 0.8 r mul rlineto ");
  n += fprintf(output, "closepath gsave BIRCH fill grestore stroke\n");
  n += fprintf(output, "r 0 moveto r -3.5 div r -7 div rlineto ");
  n += fprintf(output, "0 r 3.5 div rlineto fill\n");
  n += fprintf(output, "gsave RED -0.4 r mul 0 0.6 r mul ");
  n += fprintf(output, "0.17 r mul rect stroke grestore\n");
  n += fprintf(output, "-0.4 r mul 0 0.6 r mul 0.4 r mul rect stroke\n");
  n += fprintf(output, "grestore\n} def\n");

  /* Slash. */
  n += fprintf(output, "\n%% slash\n");
  n += fprintf(output, "/slash {\n");
  n += fprintf(output, "gsave /r exch def /y exch def /x exch def\n");
  n += fprintf(output, "x y 0.7 r mul circle ");
  n += fprintf(output, "stroke\n");
  n += fprintf(output, "x y moveto 0.8 r mul dup rmoveto\n");
  n += fprintf(output, "-1.6 r mul dup rlineto stroke\n");
  n += fprintf(output, "grestore\n} def\n");

  output_size += n;
}

/*
 *
 */
static void
get_color(char* rgb, int code) {
  if(code < 0) code = 0;
  code %= 1000000;
  rgb[0] = (char)(code/10000);
  code %= 10000;
  rgb[1] = (char)(code/100);
  code %= 100;
  rgb[2] = (char)code;
}
