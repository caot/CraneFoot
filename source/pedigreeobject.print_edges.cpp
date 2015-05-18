/* file: pedigreeobject.print_edges.cpp
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

/*
 *
 */
void
PedigreeObject::print_edges(PostScript& ps, Family& family) {
  unsigned int i, k, j;
  float x, y;
  vector<Node> nodes = family.nodes();
  char buffer[1024];
  char* ptr;
  
  for(i = 0; i < nodes.size(); i++) {
    /* Connect parents. */
    ptr = buffer;
    y = nodes[i].y;
    ptr += sprintf(ptr, "1.5 FG %.3f %.3f M ", nodes[i].x_a, y);
    ptr += sprintf(ptr, "%.3f %.3f L S\n", nodes[i].x_b, y);
    ps.append(buffer);

    /* Draw lines to children. */
    vector<int>& children = nodes[i].children;
    for(k = 0; k < children.size(); k++) {
      j = children[k];

      /* Middle siblings. */
      if(k > 0) {
	if((k + 1) == children.size()) break;
	x = nodes[j].x_a;
	y = nodes[j].y;
	ptr = buffer;
	ptr += sprintf(ptr, "1.5 FG %.3f %.3f M ", x, y);
	ptr += sprintf(ptr, "%.3f %.3f L S\n", x, (y + 1.0));
	ps.append(buffer);
	continue;
      }

      /* Central connector. */
      x = (nodes[i].x_b - 1.0);
      y = nodes[i].y;
      ptr = buffer;
      ptr += sprintf(ptr, "1.5 FG %.3f %.3f M ", x, y);
      ptr += sprintf(ptr, "%.3f %.3f L S\n", x, (nodes[j].y + 1.0));
      ps.append(buffer);

      /* Horizontal connector. */
      x = nodes[j].x_a;
      y = nodes[j].y;
      j = children[children.size()-1];
      ptr = buffer;
      ptr += sprintf(ptr, "1.5 FG %.3f %.3f M ", x, y);
      ptr += sprintf(ptr, "%.3f %.3f L ", x, (y + 1.0));
      if(children.size() > 1) {
	ptr += sprintf(ptr, "%.3f %.3f L ", nodes[j].x_a, (y + 1.0));
	ptr += sprintf(ptr, "%.3f %.3f L ", nodes[j].x_a, y);
      }
      ptr += sprintf(ptr, "S\n");
      ps.append(buffer);      
    }
  }
}
