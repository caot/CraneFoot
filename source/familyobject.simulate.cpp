/* file: familyobject.simulate.cpp
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

#include "familyobject.h"

static void iterate(vector<Branch>&, float);

/*
 *
 */
unsigned int
FamilyObject::simulate(const float limit, const int seed,
		       const bool verbose) {
  unsigned int i;
  unsigned int n = 0;
  float rho = sqrt(1.0*(members.size())*(branches.size()));
  if(f_errors.size() > 0) return 0;
  if(branches.size() < 1) return 0;
  if(limit < 1e-2) return 0;
  
  /* Compute node positions inside branches. */
  for(i = 0; i < branches.size(); i++) {
    branches[i].connect();
    branches[i].walk();
  }
  
  /* Initial positions. */ 
  update();
  for(i = 0; i < branches.size(); i++) {
    branches[i].x = rho*rand()/RAND_MAX;
    branches[i].y = rho*rand()/RAND_MAX;
  }
  
  /* Simulation parameters. */
  unsigned int counter = 0;
  clock_t alert = clock();
  time_t start = time(NULL);
  float dt = 0.0;
  float temp0 = log(1.0 + rho);  
  float temp = temp0;
  if(seed > 0) srand(seed);
  else srand(start);

  /* Simulated annealing. */
  for(n = 0; temp > 0.05; n++) {     
    /* Check temperature. */
    if(fabs(clock() - 1.0*alert) > CLOCKS_PER_SEC/2.0) {
      alert = clock();
      if(n%2) dt += 0.5/limit;
      else dt = difftime(time(NULL), start)/limit;
      float r = exp(10*(dt - 0.6));
      if(seed <= 0) {
	temp = (1.0 - r/(1.0 + r))*temp0;
	if((dt > 0.95) && (temp > 0.05)) temp = 0.05;
      }  

      /* Progress monitor. */
      if(verbose) {
	switch(counter) {
	case 0: printf("\r|"); break;
	case 2: printf("\r/"); break;
	case 4: printf("\r-"); break;
	case 6: printf("\r\\"); break;
	}
	if(counter%2 == 0) {
	  printf("\t%s\t", f_name.c_str());
	  printf("%d\t", n);
	  if(seed <= 0) printf("%3.0f%%\t", 100.0*dt);
	  printf("%.2f ", temp);
	  fflush(stdout);
	}
	counter = (counter + 1)%8;
      }   
    }

    /* Update configuration. */
    iterate(branches, temp);
    temp *= (1.0 - 1.0/(100.0 + members.size()));
  }
  if(verbose) printf("\r%80s\r", "");
  
  /* Eliminate unnecessary gaps. */
  update();
  
  return n;
}

/*
 *
 */
static void
iterate(vector<Branch>& branches, float temp) {
  unsigned int i, j;
  unsigned int n = branches.size();
  float x, y, r;
  float box[4] = {FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX};
  vector<float> gx(n, 0.0);
  vector<float> gy(n, 0.0);
  vector<float> f;
  if(n < 2) return;  

  /* Determine geometric cost. */
  for(i = 0; i < n; i++) {
    x = branches[i].x;
    y = branches[i].y;
    if(x < box[0]) box[0] = x;
    if(y < box[1]) box[1] = y;
    x += branches[i].width;
    y += branches[i].height;
    if(x > box[2]) box[2] = x;
    if(y > box[3]) box[3] = y;
  }
  x = pow((box[2] - box[0]), 2);  
  y = pow((box[3] - box[1]), 2);  
  r = 0.5*(x + y + 1e-6);
  x /= r;
  y /= r;

  /* Compute gradients. */
  for(i = 0; i < n; i++) {
    for(j = (i + 1); j < n; j++) {
	f = branches[i].repel(branches[j]);
	gx[i] += (y + 0.2*rand()/RAND_MAX)*f[0];
	gy[i] += (x + 0.2*rand()/RAND_MAX)*f[1];
	gx[j] -= (y + 0.2*rand()/RAND_MAX)*f[0];
	gy[j] -= (x + 0.2*rand()/RAND_MAX)*f[1];
    }
    f = branches[i].attract();
    gx[i] += (0.8 + 0.2*rand()/RAND_MAX)*f[0];
    gy[i] += (0.8 + 0.2*rand()/RAND_MAX)*f[1];
  }

  /* Central attractor. */
  float w = 0.5*(box[2] - box[0] + 1e-6);
  float h = 0.5*(box[3] - box[1] + 1e-6);
  float x0 = 0.5*(box[2] + box[0]);
  float y0 = 0.5*(box[3] + box[1]);
  r = sqrt(w*w + h*h);
  for(i = 0; i < n; i++) {
    x = (x0 - branches[i].x - 0.5*(branches[i].width))/r;
    y = (y0 - branches[i].y - 0.5*(branches[i].height))/r;
    gx[i] += x;
    gy[i] += y;
  }

  /* Update positions. */
  for(i = 0; i < n; i++) {
    r = sqrt(gx[i]*gx[i] + gy[i]*gy[i]);
    if(r > temp) {
      gx[i] *= temp/r;
      gy[i] *= temp/r;
    }
    branches[i].x += (gx[i] - box[0]);
    branches[i].y += (gy[i] - box[1]);
  }
}
