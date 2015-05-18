/* file: branch.walk.cpp
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
#include "walker.h"

static walker_vertex_t member2vertex(FamilyObject*, int);

/*
 *
 */
void
Branch::walk() {
  unsigned int i, k;
  unsigned int n = vertices.size();
  walker_vertex_t* wvtx;
  vector<Member>& members = family->members;

  /* Create tree nodes. */
  wvtx = (walker_vertex_t*)malloc((n + 1)*sizeof(walker_vertex_t));
  for(i = 0; i < n; i++)
    wvtx[i] = member2vertex(family, vertices[i]);

  /* Make sure every node is connected at least to the root. */
  wvtx[n].parent = -1;
  wvtx[n].child = -1;
  wvtx[n].sibling = -1;
  wvtx[n].is_centering = 1;
  wvtx[n].width = 1.0;
  wvtx[n].height = 1.0; 
  wvtx[n].up_attach = 0.5;
  wvtx[n].down_attach = 0.5;
  wvtx[n].user_data = NULL;
  for(i = 0; i < n; i++) {
    k = (n - i - 1);
    if(wvtx[k].parent >= 0) continue;
    wvtx[k].parent = n;
    wvtx[k].sibling = wvtx[n].child;
    wvtx[n].child = k;
  }

  /* Walker II's node positioning algorithm. */
  walker(wvtx, (n + 1));
  for(i = 0; i < n; i++) {
    k = vertices[i];
    members[k].x = wvtx[i].x;
    members[k].y = wvtx[i].y;
  }
  free(wvtx);

  update();
}

/*
 *
 */
walker_vertex_t
member2vertex(FamilyObject* fo, int i) {
  int ind;
  walker_vertex_t v;
  vector<Member>& members = fo->members;
  if(i < 0) return v;
  if(i >= (int)(members.size())) return v;

  /* Default values. */
  v.parent = members[i].parent;
  v.child = members[i].child;
  v.sibling = members[i].sibling;
  v.is_centering = members[i].is_original;
  v.x = members[i].x;
  v.y = members[i].y;
  v.width = members[i].width;
  v.height = members[i].height; 
  v.up_attach = members[i].up_attach;
  v.down_attach = members[i].width;
  v.user_data = NULL;

  /* Switch to vertex addresses. */
  if((ind = v.parent) >= 0) v.parent = members[ind].vertex;
  if((ind = v.child) >= 0) v.child = members[ind].vertex;
  if((ind = v.sibling) >= 0) v.sibling = members[ind].vertex;
  if(members[i].tree < 0) return v;
  if(members[i].child < 0) return v;

  /* Find coparent. */
  if((ind = members[members[i].child].father) == i)
    ind = members[members[i].child].mother;
  v.width += members[ind].width;
  
  return v;
}
