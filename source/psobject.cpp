/* file: psobject.cpp
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

#include "psobject.h"

#define BUFFER_CAP 131072

/*
 *
 */
PSObject::PSObject(const string& s) {
  /* Default values. */
  page = 0;
  output_size = 0;
  file_size = 0;
  buffer = NULL;
  fname = s;
  output = NULL;
  parameters["BackgroundColor"] = "999999";
  parameters["Creator"] = "unknown";
  parameters["DocumentMode"] = "normal";
  parameters["FontSize"] = "10";
  parameters["ForegroundColor"] = "000000";
  parameters["PageOrientation"] = "portrait";
  parameters["PageSize"] = "letter";
  parameters["VerboseMode"] = "true";
  if(s.length() < 1) return;

  /* Open file. */
  if((output = fopen(fname.c_str(), "w")) == NULL) {
    printf("WARNING! Cannot open '%s'.\n", fname.c_str());
    return;
  }
  buffer = (char*)malloc(BUFFER_CAP);
  setvbuf(output, buffer, _IOFBF, BUFFER_CAP);
}

/*
 *
 */
PSObject::~PSObject() {
  if(output == NULL) return;
  close();
}
