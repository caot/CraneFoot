/* file: pedigreeobject.print_nodes.cpp
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

static void draw_emblem(PostScript&, Emblem&, float, float);
static void draw_text(PostScript&, vector<string>&, float, float, float);

/*
 *
 */
void
PedigreeObject::print_nodes(PostScript& ps, Family& family) {
  unsigned int i;
  float font_size = atof(ps["FontSize"].c_str());
  float font_width = font_size*BASE_FONT_WIDTH;
  vector<Node> nodes = family.nodes();

  /* Draw nodes. */
  ps.append("COUR_BOLD\n");
  for(i = 0; i < nodes.size(); i++) {
    string& key_a = nodes[i].alpha;
    string& key_b = nodes[i].beta;
    if(key_a != "") {
      Emblem& emb = emblems[key_a];
      draw_emblem(ps, emb, nodes[i].x_a, nodes[i].y);
      draw_text(ps, emb.text, nodes[i].x_a, nodes[i].y, font_width);
    }
    if(key_b != "") {
      Emblem& emb = emblems[key_b];
      draw_emblem(ps, emb, nodes[i].x_b, nodes[i].y);
      draw_text(ps, emb.text, nodes[i].x_b, nodes[i].y, font_width);
    }
  }
}

/*
 *
 */
static void
draw_emblem(PostScript& ps, Emblem& emblem, float x, float y) {
  char buffer[2048];
  float rgb[3] = {0.0, 0.0, 0.0};
  char* ptr = buffer;
  if(emblem.shape == '\0') return;

  /* Node outline. */
  int n_shapes = PostScript::shape_count();
  ptr += sprintf(ptr, "1.5 FG ");
  ptr += sprintf(ptr, "%.3f %.3f 0.5 ", x, y);
  if((emblem.shape >= 1) && (emblem.shape <= n_shapes))
    ptr += sprintf(ptr, "s.%d ", emblem.shape);
  else
    ptr += sprintf(ptr, "s.4 ");

  /* Color. */
  get_rgb(rgb, emblem.color);
  ptr += sprintf(ptr, "GS %.3f %.3f %.3f ", rgb[0], rgb[1], rgb[2]);
  ptr += sprintf(ptr, "SC F GR\n");

  /* Pattern. */
  if((emblem.pattern > 0) && (emblem.pattern < 100)) {
    ptr += sprintf(ptr, "GS clip %.3f %.3f 0.5 ", x, y);
    ptr +=  sprintf(ptr, "p.%d GR\n", emblem.pattern);
  }
  ptr += sprintf(ptr, "S\n");

  /* Slash. */
  if(emblem.slash) {
    ptr +=  sprintf(ptr, "%.3f %.3f M ", (x + 0.7), (y + 0.7));
    ptr +=  sprintf(ptr, "%.3f %.3f L ", (x - 0.7), (y - 0.7));
    ptr +=  sprintf(ptr, "S\n");
  }

  /* Arrow. */
  if(emblem.arrow) {
    ptr +=  sprintf(ptr, "%.3f %.3f M ", (x - 0.8), (y - 0.2));
    ptr +=  sprintf(ptr, "0.4 -0.18 RL -0.4 -0.18 RL\n");
    ptr +=  sprintf(ptr, "GS CL S GR 1 0.1 0.1 SC F\n");
  }

  ps.append(buffer);
}

/*
 *
 */
static void
draw_text(PostScript& ps, vector<string>& text, float x, float y, float r) {
  unsigned int i;
  char buffer[64];
  x -= 0.7;
  y -= (0.5 + 0.5*r);
  for(i = 0; i < text.size(); i++) {
    if(i == 0) {
      sprintf(buffer, "%.2f FG ", 1.2*r);
      ps.append(buffer);
    }
    sprintf(buffer, "%.3f %.3f M (", x, (y - 0.5*r*i));
    ps.append(string(buffer) + pacify(text[i]) + ") show\n");
  }
}
