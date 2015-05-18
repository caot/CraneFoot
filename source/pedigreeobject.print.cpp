/* file: pedigreeobject.print.cpp
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

static void customize(PostScript&, Table&);
static bool prepare(PostScript&, Family&, float, float, int);
static unsigned long print_topology(FILE*, Family&, int);

/*
 *
 */
void
PedigreeObject::print() {
  int i;
  int page = 1;
  int figure_limit = (int)(cfg["FigureLimit"].number(1));
  float w = 0.0;
  float h = 0.0;
  map<string, Family>::iterator pos;
  PostScript ps;

  /* Prepare main document. */
  if(verbose_mode) cout << "\nMedia instructions:\n";
  ps = PostScript(cfg["PedigreeName"][1] + ".ps");
  customize(ps, cfg);
  w = print_vlegend(ps);
  h = print_hlegend(ps);
  if(verbose_mode) cout << "\nSaving results:\n";
  if(!print_toc(ps)) {
    if(verbose_mode) 
      cout << "\tWrote " << clarify(ps.size())
	   << " bytes to '" << ps["FileName"] << "'.\n";
    return;
  }
  
  /* Print families to a single document. */
  for(pos = families.begin(); pos != families.end(); i++, pos++) {
    if((pos->second).is_consistent() == false) continue;
    if(!prepare(ps, pos->second, w, h, page++)) {
      printf("WARNING! Could not attach '%s' to main document.\n",
	     (pos->first).c_str());
      continue;
    }
    print_links(ps, pos->second);
    print_edges(ps, pos->second);
    print_nodes(ps, pos->second);
  }
  ps.close();
  if(verbose_mode)
    cout << "\tWrote " << clarify(ps.size())
	 << " bytes to '" << ps["FileName"] << "'.\n";
  
  /* Print families to separate files. */
  i = 0;
  for(pos = families.begin(); pos != families.end(); pos++) {
    if((pos->second).is_consistent() == false) continue;
    if(i >= figure_limit) break;
    if(i > 9999) break;
    
    /* Determine paper size. */
    char buffer[64];
    string s = cfg["PageSize"][2];
    if((s == "") || (s == "auto")) {
      w = ((pos->second).width() + 2*MARGIN_WIDTH);
      h = ((pos->second).height() + 2*MARGIN_WIDTH);
      if(cfg["PageOrientation"][1] == "portrait")
	sprintf(buffer, "%.2f,%.2f", w, h);
      else
	sprintf(buffer, "%.2f,%.2f", h, w);
    }
    else {
      strncpy(buffer, s.c_str(), 64);
      buffer[63] = '\0';
    }
    
    /* Create eps template. */
    ps = PostScript(cfg["PedigreeName"][1] + "_" + pos->first + ".eps");
    customize(ps, cfg);
    ps.assign("DocumentMode", "encaps"); 
    ps.assign("PageSize", string(buffer));
    ps.assign("VerboseMode", "off");
    if(!prepare(ps, pos->second, 0.0, 0.0, 0)) {
      printf("WARNING! Could not print '%s' to separate file.\n",
	     (pos->first).c_str());
      continue;
    }
    
    /* Print family graph. */
    print_edges(ps, pos->second);
    print_links(ps, pos->second);
    print_nodes(ps, pos->second);
    ps.close();
    if(verbose_mode) {
      cout << "\tWrote " << clarify(ps.size())
	   << " bytes to '" << ps["FileName"] << "'.\n";
    }
    i++;
  }
  
  /* Open topology file. */
  char iobuf[131072];
  string fname = (cfg["PedigreeName"][1] + ".topology.txt");
  FILE* output = fopen(fname.c_str(), "w");
  if(output == NULL) {
    printf("WARNING! Cannot open '%s'.\n", fname.c_str());
    return;
  }
  setvbuf(output, iobuf, _IOFBF, 131072);

  /* Save topology. */
  int ind = 0;
  unsigned long n = 0;
  n += fprintf(output, "INDEX\tFAMILY\tTREE\tALPHA\tBETA\t");
  n += fprintf(output, "X_A\tX_B\tY\n");
  for(pos = families.begin(); pos != families.end(); pos++) {
    if((pos->second).is_consistent() == false) continue;
    n += print_topology(output, pos->second, ind++);
  }
  if(verbose_mode)
    cout << "\tWrote " << clarify(n) << " bytes to '" << fname << "'.\n";
  fclose(output);
}

/*
 *
 */
static void
customize(PostScript& ps, Table& cfg) {
  unsigned int i;
  for(i = 0; i < cfg.size(); i++) {
    Row r = cfg[i];
    ps.assign(r[0], r[1]);
  }
  //string s = ("CraneFoot " + string(pedigree_VERSION));
  ps.assign("Creator", ("CraneFoot " + string(pedigree_VERSION)));
}

/*
 *
 */
static bool
prepare(PostScript& ps, Family& family, float lwidth, float lheight,
	int page) {
  float x, y;
  float r = atof(ps["FontSize"].c_str())*BASE_FONT_WIDTH/2;
  float height = family.height();
  float width = family.width();
  float scale = -1.0;
  float hscale = -1.0;
  float vscale = -1.0;
  char buffer[1024];
  char* ptr;
  if(width <= 0.0) return false;
  if(height <= 0.0) return false;
  if(width > 10000.0) return false;
  if(height > 10000.0) return false;

  /* Make page template. */
  ps.new_page();
  float margin = r;
  float text_height = (ps.height() - 2*margin);
  float text_width = (ps.width() - 2*margin);

  /* Add extra info if normal page. */
  if(page > 0) {
    margin = MARGIN_WIDTH;
    text_height = (ps.height() - 3*margin);
    text_width = (ps.width() - 2*margin);

    /* Family name. */
    ptr = buffer;
    ptr += sprintf(ptr, "1 FG /Helvetica findfont ");
    ptr += sprintf(ptr, "%.4f scalefont setfont\n", margin/2.5);
    ptr += sprintf(ptr, "%.3f ", (text_width/2 + margin));
    ptr += sprintf(ptr, "%.3f moveto ", (text_height + 2*margin));
    ptr += sprintf(ptr, "(%s) showc\n", pacify(family.name(), 32).c_str());
    ps.append(buffer);

    /* Family size. */
    ptr = buffer;
    ptr += sprintf(ptr, "1 FG /Helvetica findfont ");
    ptr += sprintf(ptr, "%.4f scalefont setfont\n", margin/4);
    ptr += sprintf(ptr, "%.3f ", (text_width/2 + margin));
    ptr += sprintf(ptr, "%.3f moveto ", (text_height + 1.6*margin));
    ptr += sprintf(ptr, "(%d individuals) showc\n", family.size());
    ps.append(buffer);
    
    /* Date stamp. */
    time_t now = time(NULL);
    char date[64];
    ptr = ctime(&now);
    sprintf(date, "%.6s %.4s", (ptr + 4), (ptr + 20));

    ptr = buffer;
    ptr += sprintf(ptr, "%.3f ", text_width);
    ptr += sprintf(ptr, "%.3f moveto ", (text_height + 2*margin));
    ptr += sprintf(ptr, "(%s) showr\n", date);
    ps.append(buffer);
    
    /* Page number. */
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f ", (text_width + margin));
    ptr += sprintf(ptr, "%.3f moveto ", (text_height + 2*margin));
    ptr += sprintf(ptr, "(%d) showr\n", page);
    ps.append(buffer);
    
    /* Try horizontal legend. */
    if(text_height > 2*lheight) {
      hscale = (text_height - lheight)/height;
      if(hscale > text_width/width)
	hscale = text_width/width;
    }  
    
    /* Try vertical legend. */
    if(text_width > 2*lwidth) {
      vscale = (text_width - lwidth)/width;
      if(vscale > text_height/height)
	vscale = text_height/height;
    }
    
    /* Determine better legend. */
    if((vscale <= 0.0) && (hscale <= 0.0)) {
      printf("WARNING! %s: Legend is too large.\n", __FILE__);
      lheight = 0.0;
      lwidth = 0.0;
    }
    else {
      if(vscale > hscale) {
	text_width -= lwidth;
	lheight = 0.0;
      }
      else {
	text_height -= lheight;
	lwidth = 0.0;
      }
    }
    if(lheight > 0.0) ps.append("HLEGEND\n");
    if(lwidth > 0.0) ps.append("VLEGEND\n");
  }

  /* Determine scale. */
  x = (width + 1e-10);
  y = (height + 1e-10);
  scale = text_width/(x + 1e-10);
  if(scale > text_height/(y + 1e-10))
    scale = text_height/(y + 1e-10);
  if(scale > 1.0) scale = 1.0;
  if(scale < 0.01) {
    printf("WARNING! %s: Canvas is too small.\n", __FILE__);
    return false;
  }

  /* Center and scale canvas. */
  x = 0.5*(text_width - width*scale);
  y = 0.5*(text_height - height*scale);
  if((x > 1e-6) || (y > 1e-6)) {
    x += (lwidth + margin);
    y += (lheight + margin);
    sprintf(buffer, "%.2f %.2f translate\n", x, y);
    ps.append(buffer);
  }
  if(scale < 1.0) {
    sprintf(buffer, "%f %f scale\n", scale, scale);
    ps.append(buffer);
  }

  return true;
}

/*
 *
 */
static unsigned long
print_topology(FILE* output, Family& family, int ind) {
  unsigned int i;
  unsigned long n = 0;
  string fam_name = family.name();
  vector<Node> nodes = family.nodes();

  for(i = 0; i < nodes.size(); i++) {
    n += fprintf(output, "%d_%d", ind, nodes[i].index);
    n += fprintf(output, "\t%s", fam_name.c_str());
    n += fprintf(output, "\t%d_%d", ind, nodes[i].tree);
    n += fprintf(output, "\t%s", (nodes[i].alpha).c_str());
    n += fprintf(output, "\t%s", (nodes[i].beta).c_str());
    n += fprintf(output, "\t%7.2f", nodes[i].x_a);
    n += fprintf(output, "\t%7.2f", nodes[i].x_b);
    n += fprintf(output, "\t%7.2f", nodes[i].y);
    n += fprintf(output, "\n");
  }
  
  return n;
}
