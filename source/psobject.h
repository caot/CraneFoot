/* file: psobject.h
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

#ifndef psobject_INCLUDED
#define psobject_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

#define postscript_VERSION "1.1.1"
#define CM2DOT_ps           28.34645669
#define A0_WIDTH            84.1
#define A0_HEIGHT          118.9
#define A1_WIDTH            59.4 
#define A1_HEIGHT           84.1
#define A2_WIDTH            42.0 
#define A2_HEIGHT           59.4
#define A3_WIDTH            29.7 
#define A3_HEIGHT           42.0
#define A4_WIDTH            21.0 
#define A4_HEIGHT           29.7
#define A5_WIDTH            14.8 
#define A5_HEIGHT           21.0
#define LETTER_WIDTH        21.59
#define LETTER_HEIGHT       27.94
#define N_PATTERNS          100
#define N_SHAPES            8

using namespace std;

class PSObject {
private:
  unsigned int page;
  unsigned long output_size;
  unsigned long file_size;
  char* buffer;
  FILE* output;
  string fname;
  map<string, string> parameters;
  bool new_document();
  float paper_height();
  float paper_width();
  void prolog();
public:
  PSObject(const string&);
  ~PSObject();
  string operator[](const string&);
  bool append(const string&);
  bool assign(const string&, const string&);
  bool close();
  float height();
  bool new_page();
  unsigned long size() const;
  float width();
};

#endif /* psobject_INCLUDED */
