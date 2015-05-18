/* file: pedigreeobject.configure.cpp
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

static void detect(map<string, Locus>&, Table&, bool);
static void detect_text(vector<Locus>&, Table&, bool);

/*
 *
 */
bool
PedigreeObject::configure() {
  char delim = '\t';
  unsigned int i;
  string s, ped_file;
  vector<Locus> loci;
  Table data;

  /* Determine delimiter. */
  s = (cfg["Delimiter"][1] + "\t");
  delim = s[0];
  if(s == "tab\t") delim = '\t';
  if(s == "ws\t") delim = '\0';
  if(verbose_mode) {
    if(delim == '\0')
      cout << "\nLoading white space delimited data:\n";
    else if(delim == '\t')
      cout << "\nLoading tabulator delimited data:\n";
    else
      cout << "\nLoading '" << delim << "' delimited data:\n";
  }

  /* Copy data from primary pedigree file. */
  ped_file = cfg.getPedigreeFilename();
  data = Table(ped_file, delim);
  if(data.size() < 1) {
    cout << "WARNING! File '" << ped_file
	 << "' is empty or cannot be opened.\n";
    return false;
  }
  if(verbose_mode) {
    cout << "\tCopied " << clarify(data.mem_size()) << " bytes from '"
	 << ped_file << "'.\n";
  }

  /* Check that pedigree file contains necessary variables. */
  if(data.column(cfg["NameVariable"][1]) < 0) {
    cout << "WARNING! Could not find name variable in '"
	 << ped_file << "'.\n";
    return false;
  }
  if(data.column(cfg["FatherVariable"][1]) < 0) {
    cout << "WARNING! Could not find father variable in '"
	 << ped_file << "'.\n";
    return false;
  }
  if(data.column(cfg["MotherVariable"][1]) < 0) {
    cout << "WARNING! Could not find mother variable in '"
	 << ped_file << "'.\n";
    return false;
  }
  tables[ped_file] = data;

  /* Read variable instructions. */
  loci.push_back(Locus("AgeVariable", cfg["AgeVariable"]));
  loci.push_back(Locus("ArrowVariable", cfg["ArrowVariable"]));
  loci.push_back(Locus("ColorVariable", cfg["ColorVariable"]));
  loci.push_back(Locus("SubgraphVariable", cfg["SubgraphVariable"]));
  loci.push_back(Locus("FatherVariable", cfg["FatherVariable"]));
  loci.push_back(Locus("GenderVariable", cfg["GenderVariable"]));
  loci.push_back(Locus("MotherVariable", cfg["MotherVariable"]));
  loci.push_back(Locus("NameVariable", cfg["NameVariable"]));
  loci.push_back(Locus("PatternVariable", cfg["PatternVariable"]));
  loci.push_back(Locus("ShapeVariable", cfg["ShapeVariable"]));
  loci.push_back(Locus("SlashVariable", cfg["SlashVariable"]));
  loci.push_back(Locus("TracerVariable", cfg["TracerVariable"]));
  for(i = 0; i < 9999; i++) {
    Row row = cfg.get("TextVariable", i);
    if(row.size() < 2) break;
    loci.push_back(Locus("TextVariable", row));    
  }

  /* Associate variables with input files. */
  for(i = 0; i < loci.size(); i++) {
    string locus_name = loci[i].name();
    string& file_name = loci[i].file_name;
    if(file_name == "") file_name = ped_file;
    if(locus_name == "TextVariable") {
      text_variables.push_back(loci[i]);
      continue;
    }
    if(locus_name == "NameVariable") file_name = ped_file;
    if(locus_name == "FatherVariable") file_name = ped_file;
    if(locus_name == "MotherVariable") file_name = ped_file;
    variables[file_name][locus_name] = loci[i];
  }

  /* Copy data from secondary input files. */
  map<string, map<string, Locus> >::iterator pos;
  for(pos = variables.begin(); pos != variables.end(); pos++) {
    if(tables.count(pos->first) > 0) continue;
    data = Table(pos->first, delim); 
    if(data.size() < 1) {
      cout << "WARNING! File '" << pos->first
	   << "' is empty or cannot be opened.\n";
    }
    else if(verbose_mode) {
      cout << "\tCopied " << clarify(data.mem_size()) << " bytes from '"
	   << data.source() << "'.\n";
    }
    tables[pos->first] = data;
  }
  for(i = 0; i < text_variables.size(); i++) {
    s = text_variables[i].file_name;
    if(tables.count(s) > 0) continue;
    data = Table(s, delim); 
    if(data.size() < 1)
      cout << "WARNING! File '" << s << "' is empty or cannot be opened.\n";
    else if(verbose_mode) {
      cout << "\tCopied " << clarify(data.mem_size()) << " bytes from '"
	   << data.source() << "'.\n";
    }
    tables[s] = data;
  }

  /* Detect and check variables from files. */
  map<string, Table>::iterator iter;
  if(verbose_mode) cout << "\nDetecting variables:\n";
  for(iter = tables.begin(); iter != tables.end(); iter++) {
    detect(variables[iter->first], iter->second, verbose_mode);
    detect_text(text_variables, iter->second, verbose_mode);
  }

  /* Sort text variables according to their order in config file. */
  sort(text_variables.begin(), text_variables.end());

  /* Check that every table contains names. */
  s = cfg["NameVariable"][1];
  for(iter = tables.begin(); iter != tables.end(); iter++) {
    Table& t = iter->second;
    if(t.column(s) < 0) {
      cout << "WARNING! Could not find name variable in '"
	   << t.source() << "'.\n";
      return false;
    }
  }
 
  return true;
}

/*
 *
 */
static void
detect(map<string, Locus>& loci, Table& t, bool verbose) {
  map<string, Locus>::iterator iter;
  for(iter = loci.begin(); iter != loci.end(); iter++) {
    (iter->second).column = t.column((iter->second).heading);
    if(verbose) (iter->second).display();
  }
}

/*
 *
 */
static void
detect_text(vector<Locus>& loci, Table& t, bool verbose) {
  unsigned int i;
  for(i = 0; i < loci.size(); i++) {
    if(loci[i].file_name != t.source()) continue;
    loci[i].column = t.column(loci[i].heading);
    if(verbose) loci[i].display();
  }
}
