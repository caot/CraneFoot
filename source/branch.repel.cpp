/* file: branch.repel.cpp
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

#include "familyobject.h"

#define REPULSION_FIELD 0.5

static float overlap(const Branch*, const Branch*);

/*
 *
 */
vector<float>
Branch::repel(const Branch& branch) {
  float amp, r, dx, dy;
  vector<float> g(2, 0.0);

  if(family != branch.family) {
    fprintf(stderr, "ERROR! %s at line %d: Illegal program state.\n",
	    __FILE__, __LINE__);
    exit(1);
  }
  if(this == &branch) return g;

  /* Compute gradient. */
  dx = (x - branch.x);
  dy = (y - branch.y);
  dx += 0.5*(width - branch.width);
  dy += 0.5*(height - branch.height);
  r = (dx*dx + dy*dy);
  amp = overlap(this, &branch);
  g[0] = amp*dx/r;
  g[1] = amp*dy/r;

  return g;
}

/*
 * Check if bounding frames overlap.
 */
static float
overlap(const Branch* b1, const Branch* b2) {
  float xmin1 = (b1->x - REPULSION_FIELD);
  float ymin1 = (b1->y - REPULSION_FIELD);
  float xmax1 = (b1->x + b1->width + REPULSION_FIELD);
  float ymax1 = (b1->y + b1->height + REPULSION_FIELD);
  float xmin2 = (b2->x - REPULSION_FIELD);
  float ymin2 = (b2->y - REPULSION_FIELD);
  float xmax2 = (b2->x + b2->width + REPULSION_FIELD);
  float ymax2 = (b2->y + b2->height + REPULSION_FIELD);
  float dx = 0.0;
  float dy = 0.0;

  if(xmin1 > xmax2) dx = (xmin1 - xmax2); 
  if(xmin2 > xmax1) dx = (xmin2 - xmax1); 
  if(ymin1 > ymax2) dy = (ymin1 - ymax2); 
  if(ymin2 > ymax1) dy = (ymin2 - ymax1); 

  return 1./(dx*dx + dy*dy + 1e-10);
}
