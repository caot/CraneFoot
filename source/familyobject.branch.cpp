/* file: familyobject.branch.cpp
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

#define MAX_RECURSION_DEPTH 5000

struct Argument {
  int tree;
  unsigned int level;
  unsigned int depth;
  vector<int>* vertices;
  vector<Member>* members;
};

struct Bond {
  int a;
  int b;
};

static void dfs(Argument*, int, int);
static Bond new_bond(int, int);

/*
 *
 */
unsigned int
FamilyObject::branch() {
  unsigned int i;
  int index;
  vector<int> v;
  Bond bond = {-1, -1};
  Branch branch(NULL);
  Argument arg;

  if(f_errors.size() > 0) return 0;
  for(i = 0; i < members.size(); i++) {
    members[i].x = -1.0;
    members[i].y = -1.0;
    members[i].bonds = vector<int>(0);
  }

  /* Create bonds between coparents. */
  for(i = 0; i < members.size(); i++) {
    if((index = members[i].child) < 0) continue;
    bond = new_bond(members[index].father, members[index].mother);
    if(bond.a < 0) continue;
    if(bond.b < 0) continue;
    (members[bond.a].bonds).push_back(bond.b);
    (members[bond.b].bonds).push_back(bond.a);    
  }
  
  /* Create bonds between originals and duplicates i.e. pseudo-siblings
     with the same name. */
  for(i = 0; i < members.size(); i++) {
    if((index = members[i].sibling) < 0) continue;
    if(members[i].name() != members[index].name()) continue;
    bond = new_bond(i, index);
    (members[bond.a].bonds).push_back(bond.b);
    (members[bond.b].bonds).push_back(bond.a);    
  }
  
  /* Copy topology from members. */
  arg.tree = 0;
  arg.members = &(members);
  for(i = 0; i < members.size(); i++) {
    if(members[i].father >= 0) continue;
    if(members[i].mother >= 0) continue;
    if(members[i].child < 0) continue;
    if(members[i].tree >= 0) continue;
    
    branch = Branch(this);
    arg.level = 0;
    arg.depth = 0;
    arg.vertices = &(branch.vertices);
    dfs(&arg, (int)i, -1);
    
    branch.depth = arg.depth;
    branches.push_back(branch);
    arg.tree += 1;
  }

  /* Create outsider branches. */
  for(i = 0; i < members.size(); i++) {
    if(members[i].tree >= 0) continue;
    if((members[i].bonds).size() > 0) continue;
    branch = Branch(this);
    members[i].tree = arg.tree;
    members[i].vertex = 0;
    (branch.vertices).push_back(i);
    branches.push_back(branch);
    arg.tree += 1;
  }

  return branches.size();
}  

/*
 *
 */
static void
dfs(Argument* arg, int i, int parent) {
  int ind = -1;
  vector<int>* vertices = arg->vertices;
  vector<Member>* members = arg->members;

  if(i < 0) return;
  if(arg->level >= MAX_RECURSION_DEPTH) {
    fprintf(stderr, "ERROR! %s at line %d: ", __FILE__, __LINE__);
    fprintf(stderr, "Maximum recursion depth exceeded.\n");
    exit(1);
  }
  if(arg->level >= arg->depth)
    arg->depth = (arg->level + 1);

  /* Create vertex. */
  (*members)[i].x = vertices->size();
  (*members)[i].y = arg->level;
  (*members)[i].parent = parent;
  (*members)[i].tree = arg->tree;
  (*members)[i].vertex = vertices->size();
  vertices->push_back(i);

  /* Descend to children. */
  if((ind = (*members)[i].child) >= 0) {
    arg->level += 1;
    dfs(arg, ind, i);
    arg->level -= 1;
  }

  /* Cycle through siblings. */
  while((i = (*members)[i].sibling) >= 0) {
    if((*members)[i].tree >= 0) break;
    dfs(arg, i, parent);
  }
}

/*
 *
 */
static Bond
new_bond(int a, int b) {
  int ind;
  Bond bond;
  if((ind = a) > b) {
    a = b;
    b = ind;
  } 
  bond.a = a;
  bond.b = b;
  return bond;
}
