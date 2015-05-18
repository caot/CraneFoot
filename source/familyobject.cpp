/* file: familyobject.cpp
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

using namespace std;
using namespace cranefoot;

/*
 *
 */
FamilyObject::FamilyObject() {
  f_name = "";
  f_height = 0.0;
  f_width = 0.0;
  f_errors.push_back("Empty family.");
}

/*
 *
 */
FamilyObject::FamilyObject(const FamilyObject* fo) {
  unsigned int i;
  f_height = fo->f_height;
  f_width = fo->f_width;
  f_name = fo->f_name;
  f_errors = fo->f_errors;
  members = fo->members;
  branches = fo->branches;
  for(i = 0; i < branches.size(); i++)
    branches[i].family = this;
}

/*
 *
 */
FamilyObject::FamilyObject(const vector<Vertex>& graph) {
  unsigned int i;
  map<string, int> name2index;

  /* Default values. */
  f_name = "";
  f_height = 0.0;
  f_width = 0.0;
  if(graph.size() < 1) {
    f_errors.push_back("Empty family.");
    return;
  }

  /* Create a name mapping. */
  f_name = graph[0].family_name;
  for(i = 0; i < graph.size(); i++) {
    if(graph[i].family_name != f_name) {
      members.clear();
      error("Multiple family names detected.");
      return;
    }
    if((graph[i].name).length() < 1) {
      members.clear();
      error("Nameless individual detected.");
      return;
    }
    if(name2index.count(graph[i].name) > 0) {
      members.clear();
      error("Duplicate individual '" + graph[i].name + "' found.");
      return;
    }
    if(graph[i].width <= 0.0) {
      members.clear();
      error("Individual '" + graph[i].name + "' has non-positive width.");
      return; 
    }
    if(graph[i].height <= 0.0) {
      members.clear();
      error("Individual '" + graph[i].name + "' has non-positive height.");
      return; 
    }
    if(graph[i].up_attach <= 0.0) {
      members.clear();
      error("Individual '" + graph[i].name + "' has non-positive up attach.");
      return; 
    }
    name2index[graph[i].name] = i;
    members.push_back(Member(graph[i].name));
  }

  /* Find parents and set dimensions. */
  for(i = 0; i < graph.size(); i++) {
    if(name2index.count(graph[i].father) > 0)
      members[i].father = name2index[graph[i].father];
    if(name2index.count(graph[i].mother) > 0)
      members[i].mother = name2index[graph[i].mother];
    members[i].age = graph[i].age;
    members[i].height = graph[i].height;
    members[i].width = graph[i].width;
    members[i].up_attach = graph[i].up_attach;
    members[i].is_original = true;
    if(members[i].up_attach > members[i].width)
      members[i].up_attach = members[i].width;
  }
}
