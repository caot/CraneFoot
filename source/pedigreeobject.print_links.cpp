/* file: pedigreeobject.print_links.cpp
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

static void draw_arc(char*, float, float, float, float, float);
static void draw_halo(char*, float, float, float, Emblem*);

/*
 *
 */
void
PedigreeObject::print_links(PostScript& ps, Family& family) {
  unsigned int i, k, j, ind;
  vector<Node> nodes = family.nodes();
  Emblem emb_null;
  char buffer[1024];

  /* Randomize order to avoid biased overlaps. */
  int n_trees = 0;
  vector<unsigned int> order(nodes.size());
  for(i = 0; i < nodes.size(); i++)
    order[i] = i;
  for(i = 0; i < nodes.size(); i++) {
    k = rand()%(nodes.size());
    ind = order[k];
    order[k] = order[i];
    order[i] = ind;
    if(n_trees <= nodes[i].tree)
      n_trees = (nodes[i].tree + 1);
  }

  /* Draw links to other nodes. */
  for(ind = 0; ind < nodes.size(); ind++) {
    i = order[ind];
    if(nodes[i].origin_a == false) continue;
    float hue = 1.0*(nodes[i].tree)/n_trees;
    vector<int>& links = nodes[i].links;
    for(k = 0; k < links.size(); k++) {
      j = links[k];

      /* Extra family unit. */
      if(nodes[j].alpha == nodes[i].alpha) {
	draw_arc(buffer, nodes[j].x_a, nodes[j].y,
		 nodes[i].x_a, nodes[i].y, hue);
	ps.append(buffer);
	continue;
      }

      /* Ordinary duplicate. */
      if(nodes[j].beta == nodes[i].alpha) {
	draw_arc(buffer, nodes[j].x_b, nodes[j].y,
		 nodes[i].x_a, nodes[i].y, hue);
	ps.append(buffer);
	continue;
      }

      /* Not part of any blood line in the tree. */
      draw_arc(buffer, nodes[j].x_b, nodes[j].y,
	       nodes[i].x_b, nodes[i].y, hue);
      ps.append(buffer);
    }
  }

  /* Draw background halos. */
  vector<bool> haloflags_a(nodes.size(), false);
  vector<bool> haloflags_b(nodes.size(), false);
  for(i = 0; i < nodes.size(); i++) {
    if(nodes[i].origin_a == false) continue;

    float hue = 1.0*(nodes[i].tree)/n_trees;
    string& alpha = nodes[i].alpha;
    string& beta = nodes[i].beta;
    vector<int>& links = nodes[i].links;
    for(k = 0; k < links.size(); k++) {
      j = links[k];
      Emblem* emb = &emb_null;
        
      /* Extra family unit. */
      if(emblems.count(alpha) > 0) emb = &(emblems[alpha]);
      if(nodes[j].alpha == nodes[i].alpha) {
	if(!haloflags_a[j]) {
	  draw_halo(buffer, nodes[j].x_a, nodes[j].y, hue, emb);
	  ps.append(buffer);
	}
	if(!haloflags_a[i]) {
	  draw_halo(buffer, nodes[i].x_a, nodes[i].y, hue, emb);
	  ps.append(buffer);
	}
	haloflags_a[j] = true;
	haloflags_a[i] = true;
	continue;
      }
      
      /* Ordinary duplicate. */
      if(nodes[j].beta == nodes[i].alpha) {
	if(!haloflags_b[j]) {
	  draw_halo(buffer, nodes[j].x_b, nodes[j].y, hue, emb);
	  ps.append(buffer);
	}
	if(!haloflags_a[i]) {
	  draw_halo(buffer, nodes[i].x_a, nodes[i].y, hue, emb);
	  ps.append(buffer);
	}
	haloflags_b[j] = true;
	haloflags_a[i] = true;
	continue;
      }

      /* Not part of any blood line in the tree. */
      if(emblems.count(beta) > 0) emb = &(emblems[beta]);
      if(!haloflags_b[j]) {
	draw_halo(buffer, nodes[j].x_b, nodes[j].y, hue, emb);
	ps.append(buffer);
      }
      if(!haloflags_b[i]) {
	draw_halo(buffer, nodes[i].x_b, nodes[i].y, hue, emb);
	ps.append(buffer);
      }
      haloflags_b[j] = true;
      haloflags_b[i] = true;
    }
  }
}

/*
 *
 */
static void
draw_arc(char* buffer, float x1, float y1, float x2, float y2, float ind) {
  float x0, y0;
  float dx, dy;
  float r, phi;
  char* ptr = buffer;
  float rgb[3] = {0.0, 0.0, 0.0};

  /* Compute circle center. */
  dx = (x2 - x1);
  dy = (y2 - y1);
  r = sqrt(dx*dx + dy*dy);
  phi = atan(fabs(dy)/(fabs(dx) + 1e-10));
  if((dx >= 0.0) && (dy < 0.0)) {phi *= -1; rgb[0] = 1.0;}
  if((dx < 0.0) && (dy < 0.0)) {phi += M_PI; rgb[1] = 1.0;}
  if((dx < 0.0) && (dy >= 0.0)) {phi = (M_PI - phi); rgb[2] = 1.0;}
  x0 = (r*cos(phi + M_PI/3) + x1);
  y0 = (r*sin(phi + M_PI/3) + y1);

  /* Compute sector angles. */
  dx = (x1 - x0);
  dy = (y1 - y0);
  phi = atan(fabs(dy)/(fabs(dx) + 1e-10));
  if((dx >= 0.0) && (dy < 0.0)) phi *= -1;
  if((dx < 0.0) && (dy < 0.0)) phi += M_PI;
  if((dx < 0.0) && (dy >= 0.0)) phi = (M_PI - phi);

  /* Draw arc. */
  phi *= 180/M_PI;
  get_index_color(rgb, ind);
  ptr += sprintf(ptr, "1 FG %.3f %.3f %.3f SC ", rgb[0], rgb[1], rgb[2]);
  ptr += sprintf(ptr, "%.2f %.2f %.2f ", x0, y0, r);
  ptr += sprintf(ptr, "%.2f %.2f arc S\n", phi, (phi + 60.0));
}

/*
 *
 */
static void
draw_halo(char* buffer, float x, float y, float ind, Emblem* emblem) {
  char* ptr = buffer;
  float rgb[3] = {0.0, 0.0, 0.0};

  /* Background halo. */
  int n_shapes = PostScript::shape_count();
  ptr += sprintf(ptr, "1 FG ");
  ptr += sprintf(ptr, "%.3f %.3f 0.667 ", x, y);
  if((emblem->shape >= 1) && (emblem->shape <= n_shapes))
    ptr += sprintf(ptr, "s.%d ", emblem->shape);
  else
    ptr += sprintf(ptr, "s.4 ");
  get_index_color(rgb, ind);
  ptr += sprintf(ptr, "GS BG F GR ");
  ptr += sprintf(ptr, "%.3f %.3f %.3f ", rgb[0], rgb[1], rgb[2]);
  ptr += sprintf(ptr, "SC S\n");
}
