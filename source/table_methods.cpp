/* file: table_methods.cpp
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

#include "table.h"

/*
 *
 */
void
Table::operator=(const Table& t) {
  Table* pt = (Table*)(&t);
  void* ptr = buffer;
  buffer = pt->buffer;
  pt->buffer = ptr;
}

/*
 *
 */
Row
Table::operator[](const int i) const {
  TableBuffer* tb = (TableBuffer*)buffer;
  if(i < 0) return Row();
  if(i >= tb->n_rows) return Row();
  RowObject r;
  r.fill(tb->rows + i);
  return r;
}

/*
 *
 */
int
Table::column(const string& heading) const {
  int i;
  char* ptr;
  row_t* header;
  TableBuffer* tb = (TableBuffer*)buffer;
  if(tb->n_rows < 1) return -1;
  if(heading.length() < 1) return -1;

  header = (row_t*)(tb->rows + tb->rank2locus[0]);
  ptr = header->line;
  for(i = 0; i < header->n_fields; i++) {
    if(strcmp(ptr, heading.c_str()) == 0) return i;
    ptr += (strlen(ptr) + 1);
  }

  return -1;
}

/*
 *
 */
unsigned int
Table::id() const {
  TableBuffer* tb = (TableBuffer*)buffer;
  return tb->id_code;
}

/*
 *
 */
unsigned int
Table::mem_size() const {
  TableBuffer* tb = (TableBuffer*)buffer;
  return tb->buf_size;
}

/*
 *
 */
void
Table::print() const {
  int i, k;
  char sep = '\t';
  char* ptr;
  TableBuffer* tb = (TableBuffer*)buffer;
  row_t* rows = tb->rows;

  if(tb->data_source != NULL) printf("\n%s:", tb->data_source);
  else printf("\n(%d):", tb->id_code);
  if(isprint(tb->separator)) sep = tb->separator;
  for(i = 0; i < tb->n_rows; i++) {
    ptr = rows[i].line;
    printf("\n[%d]", i);
    printf("\t%s", ptr);    
    ptr += (strlen(ptr) + 1);
    for(k = 1; k < rows[i].n_fields; k++) {
      printf("%c%s", sep, ptr);    
      ptr += (strlen(ptr) + 1);
    }
  }
  printf("\n");
}

/*
 *
 */
unsigned int
Table::size() const {
  TableBuffer* tb = (TableBuffer*)buffer;
  return tb->n_rows;
}

/*
 *
 */
string
Table::source() const {
  TableBuffer* tb = (TableBuffer*)buffer;
  if(tb->data_source == NULL) return ""; 
  return string(tb->data_source);
}
