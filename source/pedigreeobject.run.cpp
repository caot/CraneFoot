/* file: pedigreeobject.run.cpp
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
bool
PedigreeObject::run() {
  int seed = (int)(cfg["RandomSeed"].number(1));
  time_t start = time(NULL);
  float time_limit = (float)(cfg["TimeLimit"].number(1));
  map<string, Family>::iterator pos;

  if(emblems.size() < 1) return false;
  if(families.size() < 1) return false;
  if(cfg["TimeLimit"].size() < 1)
    time_limit = 5.0*(families.size());

  if(verbose_mode) cout << "\nComputing layout:\n";
  for(pos = families.begin(); pos != families.end(); pos++) {
    Family& fam = pos->second;
    time_t now = time(NULL);
    float grace = time_limit*(fam.size())/(emblems.size());
    unsigned int n = fam.simulate(grace, seed, verbose_mode);
    if(verbose_mode) { 
      if(n > 0)
	cout << '\t' << fam.name() << '\t' << n << '\t' 
	     << "iterations in " << difftime(time(NULL), now) << "s\n";
      else
	cout << "\t...\n";
    }
  } 
  if(verbose_mode) { 
    cout << "\tLayout computed in "
	 << difftime(time(NULL), start) << "s.\n";
  }

  return true;
}
