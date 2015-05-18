/* file: postscript.cpp
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

#include "scriptum.h"
#include "psobject.h"

using namespace std;
using namespace scriptum;


/*
 *
 */
PostScript::PostScript() {
  buffer = new PSObject("");
}

/*
 *
 */
PostScript::PostScript(const PostScript& ps) {
  PostScript* pps = (PostScript*)(&ps);
  buffer = pps->buffer;
  pps->buffer = new PSObject("");
}

/*
 *
 */
PostScript::PostScript(const string& fname) {
  buffer = new PSObject(fname);
}

/*
 *
 */
PostScript::~PostScript() {
   PSObject* po = (PSObject*)buffer;
   delete po;
}

/*
 *
 */
void
PostScript::operator=(const PostScript& ps) {
  PostScript* pps = (PostScript*)(&ps);
  void* ptr = buffer;
  buffer = pps->buffer;
  pps->buffer = ptr;
}

/*
 *
 */
string
PostScript::operator[](const string& s) {
  PSObject* po = (PSObject*)buffer;
  return (*po)[s];
}

/*
 *
 */
bool
PostScript::append(const string& s) {
  PSObject* po = (PSObject*)buffer;
  return po->append(s);
}

/*
 *
 */
bool
PostScript::assign(const string& s, const string& value) {
  PSObject* po = (PSObject*)buffer;
  return po->assign(s, value);
}

/*
 *
 */
bool
PostScript::close() {
  PSObject* po = (PSObject*)buffer;
  return po->close();
}

/*
 *
 */
float
PostScript::height() {
  PSObject* po = (PSObject*)buffer;
  return po->height();
}

/*
 *
 */
bool
PostScript::new_page() {
  PSObject* po = (PSObject*)buffer;
  return po->new_page();
}

/*
 *
 */
unsigned long
PostScript::size() const {
  PSObject* po = (PSObject*)buffer;
  return po->size();
}

/*
 *
 */
float
PostScript::width() {
  PSObject* po = (PSObject*)buffer;
  return po->width();
}

/*
 *
 */ 
unsigned int
PostScript::pattern_count() {
  return N_PATTERNS;
}

/*
 *
 */ 
unsigned int
PostScript::shape_count() {
  return N_SHAPES;
}

/*
 *
 */ 
string
PostScript::version() {
  string s = "";
  s += postscript_VERSION;
  s += "\t";
  s += __DATE__;
  return s;
}
