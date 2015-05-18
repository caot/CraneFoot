/* file: cranefoot_utilities.cpp
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

#include "cranefoot_utilities.h"

using namespace std;

/*
 *
 */
void
display(string tag, string heading, int column, bool flag) {
  if(!flag) return;
  printf("\t%-20s ", tag.c_str());
  if(heading.length() > 0) {
    heading = ("'" + heading + "'");
    printf("%-16s ", heading.c_str());
    if(column >= 0) printf("[%2d]\n", (column + 1));
    else printf("not found\n");
  }
  else
    printf("empty\n");
}

/*
 *
 */
void
get_color(char* rgb, int code) {
  if(code < 0) code = 0;
  code %= 1000000;
  rgb[0] = (char)(code/10000);
  code %= 10000;
  rgb[1] = (char)(code/100);
  code %= 100;
  rgb[2] = (char)code;
}

/*
 *
 */
void
get_hue(char* color, float value) {
  float phi;
  float base = value;
  float rgb[3];
  if(base > 1.0) base = 1.0;
  if(base < 0.0) base = 0.0;
  base = pow(base, 1.4);

  phi = (0.18 + 0.85*M_PI*base);
  rgb[0] = 1.2*fabs(pow(sin(phi + M_PI/2), 2));
  rgb[1] = fabs(pow(sin(phi + M_PI/4), 2));
  rgb[2] = fabs(pow(sin(phi - M_PI/12), 4));

  phi = sqrt(0.4 + base);
  rgb[0] /= phi;
  rgb[1] /= phi;
  rgb[2] /= phi;
  if(rgb[0] > 1.0) rgb[0] = 1.0;
  if(rgb[1] > 1.0) rgb[1] = 1.0;
  if(rgb[2] > 1.0) rgb[2] = 1.0;

  color[0] = (char)(99*rgb[0]);
  color[1] = (char)(99*rgb[1]);
  color[2] = (char)(99*rgb[2]);
}

/*
 *
 */
void
get_index_color(float* rgb, float value) {
  rgb[0] = 0.85*pow(sin(M_PI*value), 2);
  rgb[1] = 0.7*pow(sin(M_PI*(value + 0.3333333)), 2);
  rgb[2] = 0.85*pow(sin(M_PI*(value + 0.6666667)), 2);
  rgb[0] += 0.109;
  rgb[1] += 0.199;
  rgb[2] += 0.149;
}

/*
 *
 */
void
get_rgb(float* rgb, int code) {
  rgb[0] = ((code/10000)%100)/99.0;
  rgb[1] = ((code/100)%100)/99.0;
  rgb[2] = (code%100)/99.0;
}

/*
 *
 */
string
clarify(unsigned long n) {
  int exponent = 0;
  float size = 1.0*n;
  char buf[32];

  if(size >= 1000.0) {
    while(size >= 100.0) {
      size /= 1024.0;
      exponent++;
    }
    if(size < 10.0) sprintf(buf, "%4.2f?", size);
    else if(size < 100.0) sprintf(buf, "%4.1f?", size);
    switch(exponent) {
    case 1: buf[4] = 'K'; break;
    case 2: buf[4] = 'M'; break;
    case 3: buf[4] = 'G'; break;
    case 4: buf[4] = 'T'; break;
    case 5: buf[4] = 'P'; break;
    }
  }
  else
    sprintf(buf, "%4.0f ", size);

  return string(buf);
}

/*
 *
 */
string
pacify(string s) {
  char c = '\0';
  unsigned int i;
  unsigned int len = s.length();
  for(i = 0; i < len; i++) {
    c = s[i];
    if(!isprint(c)) c = '?';
    if(strchr("()\\", c) != NULL) c = ' ';
    s[i] = c;
  } 
  return s;
}

/*
 *
 */
string
pacify(string s, unsigned int n) {
  char c = '\0';
  unsigned int i;
  unsigned int len = s.length();
  if(len > n) len = n;
  for(i = 0; i < len; i++) {
    c = s[i];
    if(!isprint(c)) c = '?';
    if(strchr("()\\", c) != NULL) c = ' ';
    s[i] = c;
  } 
  return s;
}
