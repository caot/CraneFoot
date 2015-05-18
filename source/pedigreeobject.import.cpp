/* file: pedigreeobject.import.cpp
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

static void check_shape(Emblem&, string&, char);

/*
 *
 */
bool
PedigreeObject::import() {
  unsigned int i, k;
  vector<Vertex> vertices;
  vector<Emblem> ev;
  map<string, Table>::iterator tpos;
  Emblem emb_null;

  if(verbose_mode) cout << "\nImporting pedigree:\n";
   
  /* Gender coding. */
  char male = MALE;
  char female = FEMALE;
  if(cfg["Male"][1].length() > 0) male = cfg["Male"][1][0];
  if(cfg["Female"][1].length() > 0) female = cfg["Female"][1][0];

  /* Find topology variables. */
  map<string, Locus>& loci = variables[cfg["PedigreeFile"][1]];
  int var1 = loci["NameVariable"].column;
  int var2 = loci["FatherVariable"].column;
  int var3 = loci["MotherVariable"].column;
 
  /* Create graph representation. */
  string label = "";
  Table& tped = tables[cfg["PedigreeFile"][1]];
  tped.sort(vector<int>(1, var1));
  for(i = 0; i < tped.size(); i++) {
    Row r = tped[i];
    if(r.rank() < 1) continue;
    if(r[var1] == "") continue;
    if(r[var1] == label) {
      cout << "WARNING! Duplicate name '" << r[var1]
	   << "' detected in file '"
	   << tped.source() << "'.\n";
      continue;
    }
    Vertex v;
    v.name = r[var1];
    v.family_name = "family";
    v.father = r[var2];
    v.mother = r[var3];
    v.age = 0.0;
    v.width = 2.0;
    v.height = 2.0;
    v.up_attach = 1.0;
    vertices.push_back(v);
    label = v.name;
  }

  /* Determine background color. */
  int bg_color = 999999;
  string value = cfg["BackgroundColor"][1];
  if(value.length() > 0) bg_color = atoi(value.c_str());
  if(bg_color < 0) bg_color = 0;
  if(bg_color > 999999) bg_color = 999999;

  /* Create emblems. */
  emb_null.arrow = false;
  emb_null.slash = false;
  emb_null.tracer = false;
  emb_null.shape = 4;
  emb_null.color = bg_color;
  emb_null.pattern = 0;
  emb_null.text = vector<string>(text_variables.size(), "");
  ev = vector<Emblem>(vertices.size(), emb_null);
  
  /* Update emblems from input file(s). */
  for(tpos = tables.begin(); tpos != tables.end(); tpos++) {
    Table& t = tpos->second;
    
    /* Visualization variables. */
    map<string, Locus>& loci = variables[tpos->first];
    int name_var = t.column(cfg["NameVariable"][1]);
    int age_var = t.column(cfg["AgeVariable"][1]);
    int subgraph_var = loci["SubgraphVariable"].column;
    int arrow_var = loci["ArrowVariable"].column;
    int color_var = loci["ColorVariable"].column;
    int gender_var = loci["GenderVariable"].column;
    int pattern_var = loci["PatternVariable"].column;
    int shape_var = loci["ShapeVariable"].column;
    int slash_var = loci["SlashVariable"].column;
    int tracer_var = loci["TracerVariable"].column;

    /* Text variables. */
    vector<int> text_vars(text_variables.size(), -1);
    for(k = 0; k < text_variables.size(); k++) {
      if(t.source() != text_variables[k].file_name) continue;
      text_vars[k] = text_variables[k].column;
    }

    /* Import available information. */
    t.sort(vector<int>(1, name_var));
    for(i = 0; i < vertices.size(); i++) {
      int ind = t.find(vertices[i].name, name_var);
      Row r = t[ind];
      if(r.size() < 1) continue;

      /* Age and subgraph name. */
      if(age_var >= 0)
	vertices[i].age = r.number(age_var);
      if(subgraph_var >= 0) 
	vertices[i].family_name = r[subgraph_var];
      
      /* Visual features. */
      if(arrow_var >= 0) ev[i].arrow = (r.number(arrow_var) > 0.0);
      if(r[color_var].length() > 0) 
	ev[i].color = (unsigned int)(r.number(color_var));
      if(r.number(pattern_var) > 0.0) 
	ev[i].pattern = (unsigned int)(r.number(pattern_var));
      if(slash_var >= 0) ev[i].slash = (r.number(slash_var) > 0.0);
      if(tracer_var >= 0) ev[i].tracer = (r.number(tracer_var) > 0.0);

      /* Symbol shape. */
      if(r[gender_var].length() > 0) {
	if(r[gender_var][0] == male) ev[i].shape = 7;
	if(r[gender_var][0] == female) ev[i].shape = 1;
      }
      if(r.number(shape_var) > 0.0)
	ev[i].shape = (unsigned int)(r.number(shape_var));
 
      /* Text info. */
      for(k = 0; k < text_vars.size(); k++) {
	if(text_vars[k] < 0) continue;
	ev[i].text[k] = r[text_vars[k]];
      }
    }
  }    

  /* Add tracer brackets. */
  if(text_variables.size() > 0) {
    for(i = 0; i < vertices.size(); i++) {
      vector<string>& text = ev[i].text;
      if(ev[i].tracer) text[0] = ("[" + text[0] + "]");
    }
  }

  /* Get base font. */
  PostScript ps;
  float font_size = atof(ps["FontSize"].c_str());
  if(cfg["FontSize"].size() > 1) font_size = cfg["FontSize"].number(1);
  if(font_size < 1.0) font_size = 1.0;
  if(font_size > 100.0) font_size = 100.0;

  /* Update vertex dimensions. */
  float font_width = 0.24*font_size*BASE_FONT_WIDTH;
  for(i = 0; i < vertices.size(); i++) {
    float width;
    vector<string>& text = ev[i].text;
    for(k = 0; k < text.size(); k++) {
      width = (text[k].length())*font_width;
      if(width > 1.8) width += 0.2;
      if(width > vertices[i].width) vertices[i].width = width;
    }
    vertices[i].height += 2*font_width*(text.size() + 0.5);
  }
  
  /* Create emblem map. */
  for(i = 0; i < vertices.size(); i++)
    emblems[vertices[i].name] = ev[i];

  /* Create families. */
  families = Family::create(vertices);
  if(families.size() < 1) {
    cout << "WARNING! Could not find any families in '"
	 << tped.source() << "'.\n";
    return false;
  }

  /* Check shapes. */
  if(cfg["ShapeVariable"].size() < 2) {
    map<string, Family>::iterator pos;
    for(pos = families.begin(); pos != families.end(); i++, pos++) {
      vector<Node> nodes = (pos->second).nodes();
      for(i = 0; i < nodes.size(); i++) {
	string& key_a = nodes[i].alpha;
	string& key_b = nodes[i].beta;
	if(key_a != "") check_shape(emblems[key_a], key_a, nodes[i].gender_a);
	if(key_b != "") check_shape(emblems[key_b], key_b, nodes[i].gender_b);
      }
    }
  }

  /* Print report. */
  if(verbose_mode) {
    if(families.size() == 1) cout << "\tFound one family and ";
    else cout << "\tFound " << families.size() << " families and ";
    if(vertices.size() == 1) cout << "one individual.\n";
    else cout << vertices.size() << " individuals.\n";
  }
  
  return true;
}

/*
 *
 */
static void
check_shape(Emblem& emb, string& name, char gender) {
  if(emb.shape == 4) {
    if(gender == 'M') emb.shape = 7;
    if(gender == 'F') emb.shape = 1;
    return;
  }
  if((emb.shape == 1) && (gender == 'M')) {
    fprintf(stderr, "WARNING! Member '%s' should be male.\n", name.c_str());
    emb.shape = 4;
    return;
  }
  if((emb.shape == 7) && (gender == 'F')) {
    fprintf(stderr, "WARNING! Member '%s' should be female.\n", name.c_str());
    emb.shape = 4;
    return;
  }
}
