/* file: pedigreeobject.h
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

#ifndef pedigreeobject_INCLUDED
#define pedigreeobject_INCLUDED

#include <map>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include "cranefoot.h"
#include "tablet.h"
#include "scriptum.h"
#include "cranefoot_utilities.h"

#define pedigree_VERSION "3.2.3"
#define MALE             'M'
#define FEMALE           'F'
#define BASE_FONT_WIDTH   0.1
#define MARGIN_WIDTH      1.5

using namespace std;
using namespace cranefoot;
using namespace tablet;
using namespace scriptum;

struct Emblem {
  bool arrow;
  bool slash;
  bool tracer;
  char shape;
  unsigned int color;
  unsigned int pattern;
  vector<string> text;
};

class Locus {
private:
  string l_name;
public:
  int column;
  int rank;
  string heading;
  string file_name;
public:
  Locus() {
    l_name = "";
    heading = "";
    rank = -1;
    column = -1;
    file_name = "";
  };
  Locus(const string& tag, const Row& r) {
    l_name = tag;
    heading = r[1];
    rank = r.rank();
    column = -1;
    file_name = r[2];
  };
  bool operator<(const Locus& loc) const {
    if(loc.rank > rank) return true;
    return false;
  }
  void display() const {
    printf("\t%-22s ", l_name.c_str());
    if(heading.length() > 0) {
      string s = ("'" + file_name + "'");
      printf("%-16s ", s.c_str());
      s = ("'" + heading + "'");
      printf("%-16s ", s.c_str());
      if(column >= 0) printf("[%2d]\n", (column + 1));
      else printf("not found\n");
    }
    else
      printf("empty\n");
  };
  string name() const {return l_name;};
};

class PedigreeObject {
private:
  bool configure();
  bool import();
  void print_edges(PostScript&, Family&);
  float print_hlegend(PostScript&);
  void print_links(PostScript&, Family&);
  void print_nodes(PostScript&, Family&);
  bool print_toc(PostScript&);
  float print_vlegend(PostScript&);
public:
  bool verbose_mode;
  ConfigTable cfg;
  vector<Locus> text_variables;
  map<string, Table> tables;
  map<string, map<string, Locus> > variables;
  map<string, Family> families;
  map<string, Emblem> emblems;
public:
  PedigreeObject();
  PedigreeObject(const string&, const string&);
  void print();
  bool run();
  Emblem emblem(const string& s) {
    if(emblems.count(s) > 0) return emblems[s];
  };
  std::string name() {return cfg.getPedigreeName();};
  unsigned int size() {return emblems.size();};
};

#endif /* pedigreeobject_INCLUDED */
