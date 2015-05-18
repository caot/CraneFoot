/* file: pedigree.cpp
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

#include "pedigreeobject.h"

/*
 *
 */
Pedigree::Pedigree() {
  buffer = new PedigreeObject();
}

/*
 *
 */
Pedigree::Pedigree(const Pedigree& ped) {
  Pedigree* pped = (Pedigree*)(&ped);
  buffer = pped->buffer;
  pped->buffer = new PedigreeObject();
}

/*
 *
 */
Pedigree::Pedigree(const string& fname, const string& pedigreeFile) {
  buffer = new PedigreeObject(fname, pedigreeFile);
}

/*
 *
 */
Pedigree::~Pedigree() {
   PedigreeObject* po = (PedigreeObject*)buffer;
   delete po;
}

/*
 *
 */
void
Pedigree::operator=(const Pedigree& ped) {
  Pedigree* pped = (Pedigree*)(&ped);
  void* ptr = buffer;
  buffer = pped->buffer;
  pped->buffer = ptr;
}

/*
 *
 */
Family
Pedigree::operator[](const string& s) {
  PedigreeObject* po = (PedigreeObject*)buffer;
  return po->families[s];
}

/*
 *
 */
map<string, Family> 
Pedigree::families() {
  PedigreeObject* po = (PedigreeObject*)buffer;
  return po->families;
}

/*
 *
 */
string
Pedigree::name() {
  PedigreeObject* po = (PedigreeObject*)buffer;
  return po->name();
}

/*
 *
 */
bool
Pedigree::run() {
  PedigreeObject* po = (PedigreeObject*)buffer;
  if(po->size() < 3) return false;
  if(po->run()) {
    po->print();
    return true;
  }
  return false;
}

/*
 *
 */
unsigned int
Pedigree::size() {
  PedigreeObject* po = (PedigreeObject*)buffer;
  return po->size();
}

/*
 *
 */
string
Pedigree::version() {
  string s = "";
  s += pedigree_VERSION;
  s += "\t";
  s += __DATE__;
  return s;
}
