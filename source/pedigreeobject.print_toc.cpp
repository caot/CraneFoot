/* file: pedigreeobject.print_toc.cpp
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

#include "pedigreeobject.h"

#define TITLE_HEIGHT 10

struct Entry {
  int page;
  float x;
  float y;
  float width;
  Family* family;
};

static vector<Entry> get_entries(int&, map<string, Family>&, PostScript&);
static void make_title(PostScript&, map<string, Family>&, string, int);
static void make_list(PostScript&, vector<Entry>&, int);

/*
 *
 */
bool
PedigreeObject::print_toc(PostScript& ps) {
  int n_correct = 0;
  vector<Entry> entries;
  
  /* Prepare entries in the list. */
  entries = get_entries(n_correct, families, ps);
  make_title(ps, families, cfg["PedigreeName"][1], n_correct);
  make_list(ps, entries, n_correct);

  if(n_correct < 1) return false;
  return true;
}

/*
 *
 */
static vector<Entry>
get_entries(int& n_correct, map<string, Family>& families, PostScript& ps) {
  int n;
  int page = 1;
  unsigned int width = 10;
  unsigned int errwidth = 10;
  float x, y;
  float r = MARGIN_WIDTH/4;
  float text_height = (ps.height() - 2*MARGIN_WIDTH);
  float text_width = (ps.width() - 2*MARGIN_WIDTH);
  float colsep;
  map<string, Family>::iterator pos;
  vector<Entry> entries(families.size());

  /* Find longest family names. */
  for(pos = families.begin(); pos != families.end(); pos++) {
    Family& family = pos->second;
    if(family.is_consistent() == false) {
      if((family.name()).length() > errwidth)
	errwidth = (family.name()).length();
      continue;
    }
    if((family.name()).length() > width)
      width = (family.name()).length();
  }
  width = (int)(r*width);  
  errwidth = (int)(r*errwidth);  

  /* Determine column separation (non-erroneous families). */
  colsep = width;
  if(floor(text_width/colsep) < 1)
    colsep = text_width;
  else
    colsep = (text_width + MARGIN_WIDTH)/floor(text_width/colsep);

  /* Fix non-erroneous families on first page. */
  n = 0;
  x = 0.0;
  y = (text_height - TITLE_HEIGHT*r);
  for(pos = families.begin(); pos != families.end(); pos++) {
    Family& family = pos->second;
    if(family.is_consistent() == false) continue;
    
    y -= r;
    if(y < FLT_EPSILON) {
      x += colsep;
      y = (text_height - TITLE_HEIGHT*r - r);
    }
    if((x + colsep/2) > text_width) {
      page++;
      break;
    }
    entries[n].page = page;
    entries[n].x = (x + MARGIN_WIDTH);
    entries[n].y = (y + MARGIN_WIDTH);
    entries[n].width = (colsep - MARGIN_WIDTH);
    entries[n].family = &family;
    n++;
  }
  
  /* Fix remaining non-erroneous families. */
  x = 0.0;
  y = text_height;
  for(; pos != families.end(); pos++) {
    Family& family = pos->second;
    if(family.is_consistent() == false) continue;

    y -= r;
    if(y < FLT_EPSILON) {
      x += colsep;
      y = (text_height - r);
    }
    if((x + colsep/2) > text_width) {
      x = 0.0;
      y = (text_height - r);
      page++;
    }
    entries[n].page = page;
    entries[n].x = (x + MARGIN_WIDTH);
    entries[n].y = (y + MARGIN_WIDTH);
    entries[n].width = (colsep - MARGIN_WIDTH);
    entries[n].family = &family;
    n++;
  }
  n_correct = n;

  /* Determine column separation (erroneous families). */
  colsep = errwidth;
  colsep += 16*r;

  /* Fix erroneous families. */
  page++;
  x = 0.0;
  y = text_height;
  for(pos = families.begin(); pos != families.end(); pos++) {
    Family& family = pos->second;
    if(family.is_consistent() == true) continue;

    y -= r;
    if(y < FLT_EPSILON) {
      x += colsep;
      y = (text_height - r);
    }
    if((x + colsep) > text_width) {
      x = 0.0;
      y = (text_height - r);
      page++;
    }
    entries[n].page = page;
    entries[n].x = (x + MARGIN_WIDTH);
    entries[n].y = (y + MARGIN_WIDTH);
    entries[n].width = (colsep - 19*r);
    entries[n].family = &family;
    n++;
  }

  return entries;
}

/*
 *
 */
static void
make_title(PostScript& ps, map<string, Family>& families, string ped_name,
	   int n_correct) {
  unsigned int n_indv = 0;
  unsigned int n_total = 0;
  time_t now = time(NULL);
  float y;
  float r = MARGIN_WIDTH/5;
  char date[64];
  char buffer[65536];
  char* ptr;

  /* Store date stamp. */
  ptr = ctime(&now);
  sprintf(date, "%.6s %.4s", (ptr + 4), (ptr + 20));

  /* Count individuals. */
  map<string, Family>::iterator pos;
  for(pos = families.begin(); pos != families.end(); pos++) {
    Family& family = pos->second;
    n_total += family.size();
    if((family.errors()).size() > 0) continue;
    n_indv += family.size();
  }

  /* Print title. */
  ps.new_page();
  y = (ps.height() - MARGIN_WIDTH - r);
  ptr = buffer;
  ptr += sprintf(ptr, "1 FG /Helvetica-Bold findfont ");
  ptr += sprintf(ptr, "%.4f scalefont setfont\n", 2*r);
  ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
  ptr += sprintf(ptr, "%.3f 0 (CraneFoot ) ashow ", 0.1*r);
  ptr += sprintf(ptr, "(%s) show\n", pedigree_VERSION);
  y -= r;
  ptr += sprintf(ptr, "/Helvetica-Italic findfont ");
  ptr += sprintf(ptr, "%.4f scalefont setfont\n", r);
  ptr += sprintf(ptr, "%.3f %.3f moveto (", MARGIN_WIDTH, (y - 0.5*r));
  ptr += sprintf(ptr, "Version date %s) show\n", __DATE__);

  /* Print summary. */
  y -= 3*r;
  ptr += sprintf(ptr, "/Helvetica findfont ");
  ptr += sprintf(ptr, "%.4f scalefont setfont\n", r);
  ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
  ptr += sprintf(ptr, "(Created) show\n");
  ptr += sprintf(ptr, "%.3f %.3f moveto ", 15*r, y);
  ptr += sprintf(ptr, "(%s) show\n", date);
  y -= 1.25*r;
  ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
  ptr += sprintf(ptr, "(Pedigree name) show\n");
  ptr += sprintf(ptr, "%.3f %.3f moveto ", 15*r, y);
  ptr += sprintf(ptr, "(%s) show\n", pacify(ped_name, 32).c_str());
  y -= 1.25*r;
  ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
  ptr += sprintf(ptr, "(Pedigree file) show\n");
  ptr += sprintf(ptr, "%.3f %.3f moveto ", 15*r, y);
  ptr += sprintf(ptr, "(%s) show\n", pacify(ps["FileName"], 32).c_str());
  y -= 1.25*r;
  ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
  ptr += sprintf(ptr, "(Num of families) show\n");
  ptr += sprintf(ptr, "%.3f %.3f moveto ", 15*r, y);
  ptr += sprintf(ptr, "(%d / %d) show\n", n_correct, (int)(families.size()));
  y -= 1.25*r;
  ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
  ptr += sprintf(ptr, "(Num of individuals) show\n");
  ptr += sprintf(ptr, "%.3f %.3f moveto ", 15*r, y);
  ptr += sprintf(ptr, "(%d / %d) show\n", n_indv, n_total);
  ps.append(buffer);
}


/*
 *
 */
static void
make_list(PostScript& ps, vector<Entry>& entries, int n_correct) {
  unsigned int i;
  int page = 1;
  float x, y;
  float r = MARGIN_WIDTH/5;
  float text_height = (ps.height() - 2*MARGIN_WIDTH);
  float text_width = (ps.width() - 2*MARGIN_WIDTH);
  char buffer[65536];
  char* ptr = buffer;

  if(n_correct < 1) {
    y = (text_height - TITLE_HEIGHT*r);
    ptr += sprintf(ptr, "1 FG /Helvetica findfont ");
    ptr += sprintf(ptr, "%.4f scalefont setfont\n", 1.5*r);
    ptr += sprintf(ptr, "RED %.3f %.3f moveto ", MARGIN_WIDTH, y);
    ptr += sprintf(ptr, "(No drawable families detected!) show\n");
    y -= 2*r;
    ptr += sprintf(ptr, "/Helvetica findfont ");
    ptr += sprintf(ptr, "%.4f scalefont setfont\n", r);
    ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
    ptr += sprintf(ptr, "(Check that input files are not empty.) show\n");
    y -= 1.25*r;
    ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
    ptr += sprintf(ptr, "(Check that field delimiter is correct.) show\n");
    y -= 1.25*r;
    ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
    ptr += sprintf(ptr, "(Check that all variable names are unique.) show\n");
    y -= 1.25*r;
    ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
    ptr += sprintf(ptr, "(Check configured column indices.) show\n");
    y -= 1.25*r;
    ptr += sprintf(ptr, "%.3f %.3f moveto ", MARGIN_WIDTH, y);
    ptr += sprintf(ptr, "(Check that family and id ");
    ptr += sprintf(ptr, "variables contain data.) show\n");
    ps.append(buffer);

    if(entries.size() < 1) return;
    page = entries[0].page;
    ps.new_page();
    ptr = buffer;
    ptr += sprintf(ptr, "1 FG %.3f ", (text_width + MARGIN_WIDTH));
    ptr += sprintf(ptr, "%.3f moveto ", (text_height + MARGIN_WIDTH + r));
    ptr += sprintf(ptr, "(ERRONEOUS FAMILIES) showr\n");
    ps.append(buffer);
  }
  ptr += sprintf(ptr, "1 FG /Helvetica findfont ");
  ptr += sprintf(ptr, "%.4f scalefont setfont\n", r);
  ps.append(buffer);  
  
  /* Print non-erroneous families. */
  for(i = 0; i < (unsigned int)n_correct; i++) {
    if(entries[i].page > page) {
      page = entries[i].page;
      ps.new_page();
      ptr = buffer;
      ptr += sprintf(ptr, "1 FG /Helvetica findfont ");
      ptr += sprintf(ptr, "%.4f scalefont setfont\n", r);
      ptr += sprintf(ptr, "%.3f ", (text_width + MARGIN_WIDTH));
      ptr += sprintf(ptr, "%.3f moveto ", (text_height + MARGIN_WIDTH + r));
      ptr += sprintf(ptr, "(TABLE OF CONTENTS) showr\n");
      ps.append(buffer);
    }
    
    Family* family = entries[i].family;     
    x = entries[i].x;
    y = entries[i].y;
    sprintf(buffer, "%.3f %.3f moveto (", x, y);
    ps.append(string(buffer) + pacify(family->name()));
    
    x += entries[i].width;
    ptr = buffer;
    ptr += sprintf(ptr, ") show\n%.3f %.3f moveto ", x, y);
    ptr += sprintf(ptr, "(%d) showr\n", (i + 1));
    ps.append(buffer);
  }

  /* Print erroneous families. */
  for(; i < entries.size(); i++) {
    if(entries[i].page > page) {
      page = entries[i].page;
      ps.new_page();
      ptr = buffer;
      ptr += sprintf(ptr, "1 FG /Helvetica findfont ");
      ptr += sprintf(ptr, "%.4f scalefont setfont\n", r);
      ptr += sprintf(ptr, "%.3f ", (text_width + MARGIN_WIDTH));
      ptr += sprintf(ptr, "%.3f moveto ", (text_height + MARGIN_WIDTH + r));
      ptr += sprintf(ptr, "(ERRONEOUS FAMILIES) showr\n");
      ps.append(buffer);
    }
    
    Family* family = entries[i].family;     
    x = entries[i].x;
    y = entries[i].y;
    sprintf(buffer, "%.3f %.3f moveto (", x, y);
    ps.append(string(buffer) + pacify(family->name()));
    
    vector<string> err = family->errors();
    x += entries[i].width;
    ptr = buffer;
    sprintf(buffer, ") show\n%.3f %.3f moveto (", x, y);
    ps.append(string(buffer) + err[0] + ") show\n");
  }
}
