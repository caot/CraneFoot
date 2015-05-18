/* file: pedigreeobject.print_hlegend.cpp
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

#define HEADING_CAP 10

/*
 *
 */
float
PedigreeObject::print_hlegend(PostScript& ps) {
  unsigned int i, k;
  unsigned int n_model = 0;
  unsigned int n_rows = 0;
  bool shape_flag = (cfg["ShapeVariable"].size() > 1);
  float x, y;
  float r = MARGIN_WIDTH/5;
  float line_factor = MARGIN_WIDTH/2;
  float width = (ps.width() - 2*MARGIN_WIDTH);
  float height = 0.0;
  float colsep = 11.25*r;
  float rgb[3] = {1.0, 1.0, 1.0};
  string s;
  char buffer[65536];
  char* ptr = buffer;

  /* Determine node model size. */
  if(!shape_flag) n_model = 5;
  else n_model = 1;
  n_model += text_variables.size(); 

  /* Count headings. */
  k = (n_model + 1);
  if(cfg["ArrowVariable"].size() > 1) k += 2;
  if(cfg["ColorVariable"].size() > 1) k += 2;
  if(cfg["FamilyVariable"].size() > 1) k += 2;
  if(cfg["FatherVariable"].size() > 1) k += 2;
  if(cfg["GenderVariable"].size() > 1) k += 2;
  if(cfg["ShapeVariable"].size() > 1) k += 2;
  if(cfg["MotherVariable"].size() > 1) k += 2;
  if(cfg["NameVariable"].size() > 1) k += 2;
  if(cfg["PatternVariable"].size() > 1) k += 2;
  if(cfg["SlashVariable"].size() > 1) k += 2;
  for(i = 0; i < 9999; i++, k += 2) {
    Row row = cfg.get("ColorInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 3) continue;
  }
  for(i = 0; i < 9999; i++, k += 2) {
    Row row = cfg.get("PatternInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 3) continue;
  }
  for(i = 0; i < 9999; i++, k += 2) {
    Row row = cfg.get("ShapeInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 3) continue;
  }

  /* Determine number of rows. */
  n_rows = (unsigned int)(k/width*colsep + 1.5);
  if(n_rows < n_model) n_rows = n_model;
  n_rows += 1;
  height = (n_rows + 3)*r;

  /* Set line width. */
  ptr = buffer;
  ptr += sprintf(ptr, "\n/HLEGEND {\n");
  ptr += sprintf(ptr, "gsave %.4f FG /Helvetica findfont ", line_factor);
  ptr += sprintf(ptr, "%.4f scalefont setfont\n", 0.75*r);
  ps.append(buffer);

  /* Print node model. */
  x = (MARGIN_WIDTH + r);
  y = height;
  if(!shape_flag) {
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f %.3f sircangle\n", (x + 2.1*r), y, 1.2*r);
    ps.append(buffer);
    y -= 2*r;
  }

  /* Set font. */
  ptr = buffer;
  ptr += sprintf(ptr, "/Helvetica findfont ");
  ptr += sprintf(ptr, "%.4f scalefont setfont\n", 0.95*r);
  ps.append(buffer);

  /* Print text headings. */
  for(i = 0; i < text_variables.size(); i++) {
    y -= r;
    s = pacify(text_variables[i].heading, HEADING_CAP);
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f moveto ", x, (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
  }
  y -= r;
  if((y/r - (int)(y/r)) < 0.4)
    y -= 0.5*r;
  
  /* Print slash heading. */
  if(y < (MARGIN_WIDTH + 0.5*r)) {
    x += colsep;
    y = height;
  }
  s = pacify(cfg["SlashVariable"][1], HEADING_CAP);
  if(s.length() > 0) {
    y -= r;
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f %.3f slash ", x, y, r);
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }
  
  /* Print tracer heading. */
  if(y < (MARGIN_WIDTH + 0.5*r)) {
    x += colsep;
    y = (height - 0.5*r);
  }
  s = pacify(cfg["TracerVariable"][1], HEADING_CAP);
  if(s.length() > 0) {
    y -= r;
    ptr = buffer;
    ptr += sprintf(ptr, "gsave /Helvetica findfont ");
    ptr += sprintf(ptr, "%.4f scalefont setfont\n", 1.4*r);
    ptr += sprintf(ptr, "%.3f %.3f moveto ", x, (y - r/3));
    ptr += sprintf(ptr, "([...]) showc grestore\n");
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }

  /* Print arrow heading. */
  if(y < (MARGIN_WIDTH + 0.5*r)) {
    x += colsep;
    y = height;
  }
  s = pacify(cfg["ArrowVariable"][1], HEADING_CAP);
  if(s.length() > 0) {
    y -= r;
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x - 0.44*r), (y + 0.5*r));
    ptr += sprintf(ptr, "%.3f %.3f rlineto ", 1.2*r, -0.5*r);
    ptr += sprintf(ptr, "%.3f %.3f rlineto\n", -1.2*r, -0.5*r);
    ptr += sprintf(ptr, "GS CL S GR 1 0.1 0.1 SC F %.4f FG\n", line_factor);
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }

  /* Print color heading. */
  if(y < (MARGIN_WIDTH + 0.5*r)) {
    x += colsep;
    y = height;
  }
  s = pacify(cfg["ColorVariable"][1], HEADING_CAP);
  if(s.length() > 0) {
    y -= r;
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f %.3f rgb ", x, y, r);
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }

  /* Print colors. */ 
  for(i = 0; i < 9999; i++) {
    Row row = cfg.get("ColorInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 3) continue;
    if(y < (MARGIN_WIDTH + 0.5*r)) {
      x += colsep;
      y = height;
    }
    y -= r;
    get_rgb(rgb, (int)(row.number(2)));
    s = pacify(row[1], HEADING_CAP);
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f %.3f circle\n", x, y, 0.75*r);
    ptr += sprintf(ptr, "gsave clip ");
    ptr += sprintf(ptr, "%f %f %f ", rgb[0], rgb[1], rgb[2]);
    ptr += sprintf(ptr, "setrgbcolor fill grestore stroke\n");
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }
  
  /* Print pattern heading. */
  if(y < (MARGIN_WIDTH + 0.5*r)) {
    x += colsep;
    y = height;
  }
  s = pacify(cfg["PatternVariable"][1], HEADING_CAP);
  if(s.length() > 0) {
    y -= r;
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f %.3f pencil ", x, y, r);
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }
     
  /* Print patterns. */ 
  for(i = 0; i < 9999; i++, k += 2) {
    Row row = cfg.get("PatternInfo", i);
    int pattern = (int)(row.number(2));
    if(pattern < 1) pattern = 1;
    if(pattern > 99) pattern = 1;
    if(row.size() < 1) break;
    if(row.size() < 3) continue;
    if(y < (MARGIN_WIDTH + 0.5*r)) {
      x += colsep;
      y = height;
    }
    y -= r;
    s = pacify(row[1], HEADING_CAP);
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f %.3f ", x, y, 0.8*r);
    ptr += sprintf(ptr, "s.7 gsave clip\n");
    ptr += sprintf(ptr, "%.3f %.3f %.3f ", x, y, 0.8*r);
    ptr += sprintf(ptr, "p.%d grestore stroke\n", pattern);
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }

  /* Print shape heading. */
  if(y < (MARGIN_WIDTH + 0.5*r)) {
    x += colsep;
    y = height;
  }
  s = pacify(cfg["ShapeVariable"][1], HEADING_CAP);
  if(s.length() > 0) {
    y -= r;
    ptr = buffer;
    ptr += sprintf(ptr, "%.3f %.3f %.3f square stroke ", x, y, 0.6*r);
    ptr += sprintf(ptr, "%.3f %.3f %.3f diamond stroke\n", x, y, 0.73*r);
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }

  /* Print shapes. */
  int n_shapes = PostScript::shape_count(); 
  for(i = 0; i < 9999; i++, k += 2) {
    Row row = cfg.get("ShapeInfo", i);
    int shape = (int)(row.number(2));
    if(row.size() < 1) break;
    if(row.size() < 3) continue;
    if(y < (MARGIN_WIDTH + 0.5*r)) {
      x += colsep;
      y = height;
    }
    y -= r;
    s = pacify(row[1], HEADING_CAP);
    ptr = buffer;
    if((shape >= 1) && (shape <= n_shapes)) {
      ptr += sprintf(ptr, "%.3f %.3f %.3f ", x, y, 0.6*r);
      ptr += sprintf(ptr, "s.%d stroke\n", shape);
    }
    else {
      ptr += sprintf(ptr, "%.3f %.3f moveto (?) show\n", x, (y - r/3));
    }
    ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 1.7*r), (y - r/3));
    ptr += sprintf(ptr, "(%s) show\n", s.c_str());
    ps.append(buffer);
    y -= r;
  }

  ptr = buffer;
  ptr += sprintf(ptr, "grestore\n} def\n");
  ps.append(buffer);

  return height;
}
