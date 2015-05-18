/* file: branch.attract.cpp
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

static void gforce(vector<float>&, float, float, float, float);
static void rforce(vector<float>&, float, float, float, float);

/*
 *
 */
vector<float>
Branch::attract() {
  unsigned int i, k, n;
  int ind1, ind2;
  vector<Member>& members = family->members;
  vector<Branch>& branches = family->branches;
  vector<float> g(2, 0.0);

  if(vertices.size() == 0) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Illegal program state.\n");
    exit(1);
  }
  if(branches.size() < 1) return g;

  /* Connections with other branches. */
  n = 0;
  for(i = 0; i < bondings.size(); i++) {
    ind1 = bondings[i];
    vector<int>& bonds = members[ind1].bonds;
    for(k = 0; k < bonds.size(); k++) {
      ind2 = bonds[k];
      if(members[ind2].tree == members[ind1].tree) continue;
      if(members[ind2].tree < 0) continue;
      Branch& b2 = branches[members[ind2].tree];
      gforce(g, (x + members[ind1].x), (y + members[ind1].y),
	     (b2.x + members[ind2].x), (b2.y + members[ind2].y));
      n++;
    }
  }

  /* Normalize gradient. */
  g[0] /= (1.0 + n);
  g[1] /= (1.0 + n);
  
  /* Create random connection to ensure that branches stay together.*/
  if(1.0*rand()/RAND_MAX*n < 1.0) {
    ind1 = (rand())%(branches.size());
    Branch& b = branches[ind1];
    rforce(g, (x + 0.5*width), (y + 0.5*height),
	   (b.x + 0.5*(b.width)), (b.y + 0.5*(b.height)));
  }
  
  return g;
}

/*
 *
 */
static void
gforce(vector<float>& g, float x1, float y1, float x2, float y2) {
  float dx = (x2 - x1);
  float dy = (y2 - y1);
  float r = sqrt(dx*dx + dy*dy);
  dx /= (r + 1e-6);
  dy /= (r + 1e-6);
  if(r > 4.0) r = (4.0 + sqrt(r - 4.0));
  g[0] += dx*r;
  g[1] += dy*r;
}

/*
 *
 */
static void
rforce(vector<float>& g, float x1, float y1, float x2, float y2) {
  float dx = (x2 - x1);
  float dy = (y2 - y1);
  float r = sqrt(dx*dx + dy*dy);
  dx /= (r + 1e-6);
  dy /= (r + 1e-6);
  if(r > 2.0) r = (2.0 + log(r - 1.0));
  g[0] += dx*r;
  g[1] += dy*r;
}
