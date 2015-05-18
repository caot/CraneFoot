/* file: familyobject.link.cpp
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

struct MEntry {
  int index;
  int father;
  int mother;
  float age;
};

class CompareParent {
public:
  bool operator()(const MEntry& mb1, const MEntry& mb2) const {
    if(mb1.father < mb2.father) return true;
    if(mb1.mother < mb2.mother) return true;     
    return false; 
  };
};

class CompareAge {
public:
  bool operator()(const MEntry& mb1, const MEntry& mb2) const {
    if(mb1.age > mb2.age) return true;
    return false;
  };
};

static void sort_subset(vector<int>&, vector<Member>&, string);

/*
 * Find full siblings, remove redundant child-parent links.
 * Note the use of 'bonds' as a temporary child list.
 */
unsigned int
FamilyObject::link(const bool sibcheck) {
  unsigned int i, k;
  unsigned int n_nuclei = 0;
  int parent, coparent, stepparent;
  int sib, prevsib;

  /* Check topology and clear child lists. */
  if(!check(sibcheck)) return 0;
  for(i = 0; i < members.size(); i++)
    (members[i].bonds).clear();
  
  /* Link every child to descendant parent if possible. */
  for(i = 0; i < members.size(); i++) {
    if((parent = members[i].mother) >= 0) {
      if(members[parent].mother >= 0) {
	(members[parent].bonds).push_back(i);	
	continue;
      }
    }
    if((coparent = members[i].father) >= 0) {
      if(members[coparent].mother >= 0) {
	(members[coparent].bonds).push_back(i);
	continue;
      }
    }
    if(parent >= 0) { 
      (members[parent].bonds).push_back(i);
      continue;
    }
    if(coparent >= 0) 
      (members[coparent].bonds).push_back(i);
  }
  
  /* Link siblings and remove non-first borns. */
  for(i = 0; i < members.size(); i++) {
    prevsib = -1;
    coparent = -1;
    sort_subset(members[i].bonds, members, "parent");
    for(k = 0; k < (members[i].bonds).size(); k++) {
      sib = (members[i].bonds)[k];
      members[sib].sibling = -1;
      stepparent = members[sib].father;
      if(stepparent == (int)i)
	stepparent = members[sib].mother;      

      if(stepparent == coparent) {	
	if(prevsib < 0) {
	  fprintf(stderr, "ERROR! %s at line %d: Illegal program state.\n",
		  __FILE__, __LINE__);
	  exit(1);
	}
	members[prevsib].sibling = sib;
	prevsib = (members[i].bonds)[k];
	(members[i].bonds)[k] = -1; /* remove non-first born */
      }
      else {
	coparent = stepparent;
	prevsib = sib;
	n_nuclei++;
      }
    }
  }

  /* Create duplicates to make family nuclei pure. */
  for(i = 0; i < members.size(); i++) {
    if((members[i].bonds).size() < 1) continue;
    sort_subset(members[i].bonds, members, "parent");
    for(k = 1; k < (members[i].bonds).size(); k++) {
      if((members[i].bonds)[k] < 0) break;
      Member mb(members[i].name());
      mb.gender = members[i].gender;
      mb.father = members[i].father;
      mb.mother = members[i].mother;
      mb.sibling = members[i].sibling;
      mb.child = (members[i].bonds)[k];
      mb.age = members[i].age;
      mb.width = members[i].width;
      mb.height = members[i].height;
      members[i].sibling = members.size();
      members.push_back(mb);
    }
    members[i].child = (members[i].bonds)[0];
    members[i].bonds = vector<int>(0);
  }

  /* Make sure links from children to parents are correct. */
  for(i = 0; i < members.size(); i++) {
    sib = members[i].child;
    if(members[i].gender == MALE) {
      while(sib >= 0) {
	members[sib].father = i;
	sib = members[sib].sibling;
      }
    }
    else {
      while(sib >= 0) {
	members[sib].mother = i;
	sib = members[sib].sibling;
      }
    }
  }

  /* Sort siblings according to age. */
  for(i = 0; i < members.size(); i++) {
    sib = members[i].child; 
    vector<int> sibs;
    while(sib >= 0) {
      sibs.push_back(sib);
      sib = members[sib].sibling;
    }
    if(sibs.size() < 2) continue;
    sort_subset(sibs, members, "age");
    members[i].child = sibs[0];
    for(k = 1; k < sibs.size(); k++) {
      members[sibs[k-1]].sibling = sibs[k];
      members[sibs[k]].sibling = -1; 
    }
  }

  return n_nuclei;
}

/*
 *
 */
static void
sort_subset(vector<int>& subset, vector<Member>& members, string mode) {
  unsigned int k;
  unsigned int n = subset.size();
  vector<MEntry> entries; 
  if(n < 2) return;

  /* Create entries. */
  int ind;
  MEntry ent;
  for(k = 0; k < n; k++) {
    if((ind = subset[k]) < 0) continue;
    ent.index = ind;
    ent.father = members[ind].father;
    ent.mother = members[ind].mother;
    ent.age = members[ind].age;
    entries.push_back(ent);
  }

  /* Sort entries. */
  if(mode == "parent")
    sort(entries.begin(), entries.end(), CompareParent());
  if(mode == "age")
    sort(entries.begin(), entries.end(), CompareAge());
  
  /* Update subset. */
  for(k = 0; k < entries.size(); k++)
    subset[k] = entries[k].index;
  for(; k < n; k++)
    subset[k] = -1;
}
