/* file: familyobject_methods.cpp
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
FamilyObject::error(const string& s) {
  if(f_errors.size() > 9) return;
  if(f_errors.size() < 9) f_errors.push_back(s);
  else f_errors.push_back("Remaining errors suppressed.");
}

/*
 *
 */
vector<string>
FamilyObject::errors() {
  return f_errors;
}

/*
 *
 */
float
FamilyObject::height() {
  if(branches.size() < 1) return 0.0;
  if(f_height <= 0.0) update();
  return f_height;
}

/*
 *
 */
bool
FamilyObject::is_consistent() {
  if(branches.size() < 1) return false;
  if(f_errors.size() > 0) return false;
  return true;
}

/*
 *
 */
string
FamilyObject::name() {
  return f_name;
}

/*
 *
 */
vector<Node>
FamilyObject::nodes() {
  unsigned int i, k;
  int ind, tree;
  vector<Node> graph;
  vector<int> rank2index(members.size(), -1);

  /* Determine node height. */
  float height = 0.0;
  for(i = 0; i < members.size(); i++) {
    if((tree = members[i].tree) < 0) continue;
    if(members[i].height > height) height = members[i].height;
  }

  /* Create nodes. */
  Node node;
  for(i = 0; i < members.size(); i++) {
    if((tree = members[i].tree) < 0) continue;
    node.tree = tree;
    node.origin_a = members[i].is_original;
    node.origin_b = false;
    node.gender_a = members[i].gender;
    node.gender_b = '\0';
    node.x_a = (branches[tree].x + members[i].x + 1.0);
    node.x_b = node.x_a;
    node.y = (branches[tree].y + members[i].y + height);
    node.alpha = members[i].name();
    node.beta = "";
    node.children = vector<int>(0);
    node.links = vector<int>(0);

    /* Non-leaf node. */
    if(members[i].child >= 0) {
      ind = members[members[i].child].mother;
      if(ind == (int)i) ind = members[members[i].child].father;
      node.gender_b = members[ind].gender;
      node.x_b += members[i].width;
      node.beta = members[ind].name();
      
      /* Collect children. */
      ind = members[i].child;
      while(ind >= 0) {
	if(members[ind].is_original)
	  (node.children).push_back(ind);
	ind = members[ind].sibling;
      }
    }
    
    node.index = graph.size();
    rank2index[i] = node.index;
    graph.push_back(node);
  }

  /* Update addresses of children. */
  for(i = 0; i < graph.size(); i++) {
    vector<int>& children = graph[i].children;
    for(k = 0; k < children.size(); k++)
      children[k] = rank2index[children[k]];
  }

  /* Collect original nodes. */
  map<string, int> name2index;
  for(i = 0; i < graph.size(); i++) {
    if(graph[i].origin_a == false) continue;
    name2index[graph[i].alpha] = i;
  }
  
  /* Connect duplicates to originals. */
  for(i = 0; i < graph.size(); i++) {
    string& alpha = graph[i].alpha; 
    string& beta = graph[i].beta;
    if(name2index.count(alpha) > 0) {
      if((k = name2index[alpha]) != i)
	(graph[k].links).push_back(i);
    }
    if(beta.length() < 1) continue;
    if(name2index.count(beta) > 0) {
      if((k = name2index[beta]) != i)
	(graph[k].links).push_back(i);
    }
    else {
      graph[i].origin_b = true;
      name2index[beta] = i;
    }
  }

  return graph;
}

/*
 *
 */
unsigned int
FamilyObject::size() {
  return members.size();
}

/*
 *
 */
void
FamilyObject::update() {
  unsigned int i;
  float x, y;
  float box[4] = {FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX};
  vector<float> d(2, 0.0);
  if(branches.size() < 1) return;

  /* Determine bounding box. */
  for(i = 0; i < branches.size(); i++) {
    x = branches[i].x;
    y = branches[i].y;
    if(x < box[0]) box[0] = x;
    if(y < box[1]) box[1] = y;
    x += branches[i].width;
    y += branches[i].height;
    if(x > box[2]) box[2] = x;
    if(y > box[3]) box[3] = y;
  }

  /* Remove offsets. */
  for(i = 0; i < branches.size(); i++) {
    branches[i].x -= box[0];
    branches[i].y -= box[1];
  }
  f_width = (box[2] - box[0]);
  f_height = (box[3] - box[1] + 1.0);
}  

/*
 *
 */
float
FamilyObject::width() {
  if(branches.size() < 1) return 0.0;
  if(f_width <= 0.0) update();
  return f_width;
}
