/* file: familyobject.check.cpp
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
bool
FamilyObject::check(const bool sibcheck) {
  unsigned int i;
  unsigned int n_children = 0;
  string name = "";
  vector<string> anom;

  /* Check that topology is correct. */
  for(i = 0; i < members.size(); i++) {
    if(members[i].name() == name) {
      fprintf(stderr, "ERROR! %s at line %d: Illegal program state.\n",
	      __FILE__, __LINE__);
      exit(1);
    }
    name = members[i].name();

    /* Either 0 or 2 parents. */
    int father = members[i].father;
    int mother = members[i].mother;
    if((father >= 0) ^ (mother >= 0)) {
      error("'" + name + "'\tMust have either 0 or 2 parents.");
      continue;
    }
    if((father == mother) && (father >= 0)) {
      error("'" + name + "'\tParents must be distinct.");
      continue;
    }
    if(father < 0) continue;
    n_children++;    

    /* A child cannot be his own parent. */
    if((father == (int)i) || (mother == (int)i)) {
      error("'" + name + "'\tChild-parent cycle.");
      continue;
    }
    
    /* Father is male, mother is female. Set gender if unknown. */
    if(members[father].gender == FEMALE) {
      error("'" + name + "'\tFather is already mother to other children.");
      continue;
    }
    members[father].gender = MALE;
    if(members[mother].gender == MALE) {
      error("'" + name + "'\tMother is already father to other children.");
      continue;
    }
    members[mother].gender = FEMALE;
    
    /* Check if sibling couple. */
    int grandfather = members[father].father;
    int grandmother = members[mother].mother;
    if((grandfather >= 0) && (grandmother >= 0 )) {
      if((grandfather == members[mother].father) ||
	 (members[father].mother == grandmother))
	anom.push_back("'" + name + "'\tParents are siblings.");
    }

    /* Check if coupling within first-degree. */
    if(father == members[mother].father)
      anom.push_back("'" + name + "'\tGrandfather and father are the same.");
    if(mother == members[father].mother)
      anom.push_back("'" + name + "'\tGrandmother and mother are the same.");
  }
  if((n_children < 1) && (f_errors.size() < 1))
    error("empty\tNo offspring detected.");
  
  /* List errors. */
  if(f_errors.size() > 0) {
    printf("WARNING! Errors in '%s':\n", f_name.c_str());
    for(i = 0; i < f_errors.size(); i++)
      printf("\t%s\n", f_errors[i].c_str());
    return false;
  }
  if(anom.size() > 0) {
    printf("WARNING! Anomalies in '%s':\n", f_name.c_str());
    for(i = 0; i < anom.size(); i++) {
      if(i >= 9) {
	printf("\tRemaining anomalies suppressed.\n");
	break;
      }
      printf("\t%s\n", anom[i].c_str());
    }
  }

  return true;
}
