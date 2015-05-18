/* file: branch.update.cpp
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

/*
 *
 */
void
Branch::update() {
  unsigned int i, j, k;
  float xp, yp; 
  float box[4] = {FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX};
  vector<Member>& members = family->members;

  /* Determine bounding box. */  
  for(i = 0; i < vertices.size(); i++) {
    j = vertices[i];
    xp = members[j].x;
    yp = members[j].y;
    if(xp < box[0]) box[0] = xp;
    if(yp < box[1]) box[1] = yp;
    xp += members[j].width;
    yp += members[j].height;
    if(members[j].child >= 0) {
      if((k = members[members[j].child].father) == j)
	k = members[members[j].child].mother;
      xp += members[k].width;
    }
    if(xp > box[2]) box[2] = xp;
    if(yp > box[3]) box[3] = yp;
  }

  /* Eliminate offsets. */
  for(i = 0; i < vertices.size(); i++) {
    j = vertices[i];
    members[j].x -= box[0];
    members[j].y -= box[1];
  }
  width = (box[2] - box[0]);
  height = (box[3] - box[1]);
}
