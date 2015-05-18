/* file: family.cpp
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

class CompareFamily {
private:
  vector<Vertex>* g;
public:
  CompareFamily(const vector<Vertex>* v) {
    g = (vector<Vertex>*)v;
  };
  bool operator()(const int i1, const int i2) const {
    Vertex& mb1 = (*g)[i1];
    Vertex& mb2 = (*g)[i2];
    if(mb1.family_name < mb2.family_name) return true;
    return false;
  };
};

/*
 *
 */
Family::Family() {
  buffer = new FamilyObject();
}

/*
 *
 */
Family::Family(const Family& fam) {
  buffer = new FamilyObject((FamilyObject*)(fam.buffer));
}

/*
 *
 */
Family::Family(const vector<Vertex>& graph) {
  FamilyObject* fo = new FamilyObject(graph);
  fo->link(true);
  fo->branch();
  buffer = fo;
}

/*
 *
 */
Family::~Family() {
  FamilyObject* fo = (FamilyObject*)buffer;
  delete fo;
}

/*
 *
 */
void
Family::operator=(const Family& fam) {
  FamilyObject* fo = (FamilyObject*)buffer;
  delete fo;
  buffer = new FamilyObject((FamilyObject*)(fam.buffer));
}

/*
 *
 */
vector<string>
Family::errors() {
  FamilyObject* fo = (FamilyObject*)buffer;
  return fo->errors();
}

/*
 *
 */
float
Family::height() {
  FamilyObject* fo = (FamilyObject*)buffer;
  return fo->height();
}

/*
 *
 */
bool
Family::is_consistent() {
  FamilyObject* fo = (FamilyObject*)buffer;
  return fo->is_consistent();
}

/*
 *
 */
string
Family::name() {
  FamilyObject* fo = (FamilyObject*)buffer;
  return fo->name();
}

/*
 *
 */
vector<Node>
Family::nodes() {
  FamilyObject* fo = (FamilyObject*)buffer;
  return fo->nodes();
}

/*
 *
 */
unsigned int
Family::simulate(const float limit, const int seed, const bool flag) {
  FamilyObject* fo = (FamilyObject*)buffer;
  return fo->simulate(limit, seed, flag);
}

/*
 *
 */
unsigned int
Family::size() {
  FamilyObject* fo = (FamilyObject*)buffer;
  return fo->size();
}

/*
 *
 */
float
Family::width() {
  FamilyObject* fo = (FamilyObject*)buffer;
  return fo->width();
}

/*
 *
 */
map<string, Family>
Family::create(const std::vector<Vertex>& graph) {
  unsigned int i, k;
  string family_name = "";
  vector<int> view;
  vector<Vertex> subgraph;
  map<string, Family> families;
  if(graph.size() < 1) return families;

  /* Create a sorted view. */
  for(i = 0; i < graph.size(); i++) {
    if(graph[i].name == "") continue;
    if(graph[i].family_name == "") continue;
    view.push_back(i);
  }
  sort(view.begin(), view.end(), CompareFamily(&graph));
  
  /* Create families. */
  for(i = 0; i < view.size(); i++) {
    k = view[i];
    if((graph[k].name).length() < 1) continue;
    if(family_name.length() < 1) family_name = graph[k].family_name;
    if(family_name != graph[k].family_name) {
      families[family_name] = Family(subgraph);
      subgraph.clear();
      family_name = graph[k].family_name;
    }
    subgraph.push_back(graph[k]);
  }
  if(subgraph.size() > 0)
    families[family_name] = Family(subgraph);

  return families;
}

/*
 *
 */
string
Family::version() {
  string s = "";
  s += family_VERSION;
  s += "\t";
  s += __DATE__;
  return s;
}
