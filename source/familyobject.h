/* file: familyobject.h
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

#ifndef familyobject_INCLUDED
#define familyobject_INCLUDED

#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include "cranefoot.h"
#include "member.h"

#define family_VERSION "1.0.1"

using namespace std;
using namespace cranefoot;

class FamilyObject;
class Branch {
public:
  unsigned int depth;
  float x;
  float y;
  float width;
  float height;
  vector<int> bondings;
  vector<int> vertices;
  FamilyObject* family;
public:
  Branch(FamilyObject*);
  vector<float> attract();
  void connect();
  vector<float> repel(const Branch&);
  void update();
  void walk();
};

class FamilyObject {
private:
  bool check(const bool);
  void error(const string&);
  void update();
public:
  float f_height;
  float f_width;
  string f_name;
  vector<string> f_errors;
  vector<Member> members;
  vector<Branch> branches;
public:
  FamilyObject();
  FamilyObject(const FamilyObject*);
  FamilyObject(const vector<Vertex>&);
  unsigned int branch();
  vector<string> errors();
  float height();
  bool is_consistent();
  unsigned int link(const bool);
  string name();
  vector<Node> nodes();
  unsigned int simulate(const float, const int, const bool);
  unsigned int size();
  float width();
};

#endif /* familyobject_INCLUDED */
