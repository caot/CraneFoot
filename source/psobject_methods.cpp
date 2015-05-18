/* file: psobject_methods.cpp
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

static string pacify(string s);

/*
 *
 */
string
PSObject::operator[](const string& s) {
  if(s == "FileName") return fname;
  if(parameters.count(s) > 0) return parameters[s];
  return "";
}

/*
 *
 */
bool
PSObject::append(const string& s) {
  if(output == NULL) return false;
  if(output_size == 0) {
    new_document();
    prolog();
  }
  output_size += fprintf(output, "%s", s.c_str());
  return true;
}

/*
 *
 */
bool
PSObject::assign(const string& s, const string& value) {
  if(output_size > 0) {
    printf("WARNING! Parameters cannot be set for existing document.\n");
    return false;
  }
  if(value == "") return false;
  if(parameters.count(s) < 1) return false;
  parameters[s] = value;
  return true;
}

/*
 *
 */
bool
PSObject::close() {
  if(output == NULL) return false;
  if((file_size == 0) && (output_size == 0)) append("\n% empty page\n");
  output_size += fprintf(output, "\nshowpage\n");
  output_size += fprintf(output, "\n%%%%Trailer\n");
  output_size += fprintf(output, "%%%%Pages %d\n", page);
  output_size += fprintf(output, "%%%%EOF\n");
  file_size = output_size;
  fclose(output);
  free(buffer);
  page = 0;
  output_size = 0;
  buffer = NULL;
  output = NULL;
  return true;
}

/*
 *
 */
float
PSObject::height() {
  if(parameters["PageOrientation"] == "landscape")
    return paper_width();
  return paper_height();
}

/*
 *
 */
bool
PSObject::new_document() {
  int ivalue;
  float fvalue;
  unsigned long n = 0;
  time_t now = time(NULL);
  string s;
  char buffer[1024];
  char* ptr = NULL;
  if(output == NULL) return false;
  if(output_size > 0) return false;

  /* Check parameters. */
  s = parameters["BackgroundColor"];
  ivalue = atoi(s.c_str());
  if(ivalue > 999999) ivalue = 999999;
  if(ivalue < 0) ivalue = 0;
  sprintf(buffer, "%06d", ivalue);
  parameters["BackgroundColor"] = string(buffer);

  s = parameters["DocumentMode"];
  strcpy(buffer, "normal");
  if(s == "encaps") strcpy(buffer, "encaps");
  parameters["DocumentMode"] = string(buffer);

  s = parameters["ForegroundColor"];
  ivalue = atoi(s.c_str());
  if(ivalue > 999999) ivalue = 999999;
  if(ivalue < 0) ivalue = 0;
  sprintf(buffer, "%06d", ivalue);
  parameters["ForegroundColor"] = string(buffer);

  s = parameters["FontSize"];
  fvalue = atof(s.c_str());
  if(fvalue < 1.0) fvalue = 1.0;
  if(fvalue > 100.0) fvalue = 100.0;
  sprintf(buffer, "%.2f", fvalue);
  parameters["FontSize"] = string(buffer);

  s = parameters["PageOrientation"];
  strcpy(buffer, "portrait");
  if(s == "landscape") strcpy(buffer, "landscape");
  parameters["PageOrientation"] = string(buffer);

  buffer[0] = '\0';
  s = parameters["PageSize"];
  if(s == "a0") strcpy(buffer, "a0");
  if(s == "a1") strcpy(buffer, "a1");
  if(s == "a2") strcpy(buffer, "a2");
  if(s == "a3") strcpy(buffer, "a3");
  if(s == "a4") strcpy(buffer, "a4");
  if(s == "a5") strcpy(buffer, "a5");
  if(s == "letter") strcpy(buffer, "letter");
  if(atof(s.c_str()) > 1.0)
    sprintf(buffer, "%.2fcm,%.2fcm", paper_width(), paper_height());
  if((buffer[0] == '\0') && (s.length() > 0)) {
    printf("WARNING! Page size reverted to 'letter'.\n");
    strcpy(buffer, "letter");
  }
  parameters["PageSize"] = string(buffer);

  s = parameters["VerboseMode"];
  strcpy(buffer, "on");
  if(s == "off") strcpy(buffer, "off");
  parameters["VerboseMode"] = string(buffer);

  /* Show final parameter values. */
  if(parameters["VerboseMode"] == "on") {
    map<string, string>::iterator pos = parameters.begin();
    for(; pos != parameters.end(); pos++) {
      printf("\t%-22s ", (pos->first).c_str());
      printf("%-16s\n", (pos->second).c_str());
    }
  }

  /* Start new document. */ 
  ptr = ctime(&now);
  sprintf(buffer, "%.6s %.4s", (ptr + 4), (ptr + 20));
  n += fprintf(output, "%%!PS-Adobe-1.0\n%%%%Creator: ");
  n += fprintf(output, "%s, ", pacify(parameters["Creator"]).c_str());
  n += fprintf(output, "Scriptum %s %s\n", postscript_VERSION, __DATE__);
  n += fprintf(output, "%%%%CreationDate: %s\n", buffer);
  n += fprintf(output, "%%%%Title: %s\n", pacify(fname).c_str());
  if(parameters["DocumentMode"] == "encaps")
    n += fprintf(output, "%%%%Pages: 1\n");
  n += fprintf(output, "%%%%BoundingBox: 0 0 ");
  n += fprintf(output, "%.0f ", CM2DOT_ps*paper_width());
  n += fprintf(output, "%.0f\n", CM2DOT_ps*paper_height());
  n += fprintf(output, "%%%%EndComments\n");
  
  output_size += n;
  return true;
}

/*
 *
 */
bool
PSObject::new_page() {
  unsigned long n = 0;
  string s;

  if(output == NULL) return false;
  if(output_size == 0) append("");
  if(page > 0) {
    if(parameters["Mode"] == "encaps") {
      printf("WARNING! Page cannot be changed in 'encaps' mode.\n");
      return 0;
    }
    n += fprintf(output, "\nshowpage\n");
  }
  else
    n += fprintf(output, "\n%%%%EndProlog\n");

  /* Page setup. */
  page++;
  n += fprintf(output, "\n%%%%Page: %d %d\n", page, page);
  s = parameters["PageSize"];
  if(isalpha(s[0])) n += fprintf(output, "%s\n", s.c_str());

  /* Canvas setup. */
  float w = paper_width();
  float h = paper_height();
  n += fprintf(output, "FF 0.5 SF ");
  n += fprintf(output, "BG 0 0 M %.0f 0 L ", w*CM2DOT_ps);
  n += fprintf(output, "%.0f %.0f L ", w*CM2DOT_ps, h*CM2DOT_ps);
  n += fprintf(output, "0 %.0f L F\n", h*CM2DOT_ps);
  if(parameters["PageOrientation"] == "landscape") {
    n += fprintf(output, "%.0f 0 translate ", CM2DOT_ps*paper_width());
    n += fprintf(output, "90 rotate ");
  }
  n += fprintf(output, "%f %f scale\n", CM2DOT_ps, CM2DOT_ps);

  output_size += n;
  return true;
}

/*
 *
 */
float
PSObject::paper_height() {
  float h = -1.0;
  string s = parameters["PageSize"];
  if(s == "a0") h = A0_HEIGHT;
  if(s == "a1") h = A1_HEIGHT;
  if(s == "a2") h = A2_HEIGHT;
  if(s == "a3") h = A3_HEIGHT;
  if(s == "a4") h = A4_HEIGHT;
  if(s == "a5") h = A5_HEIGHT;
  if(s == "letter") h = LETTER_HEIGHT;
  if(h < 0.0) {
    char* ptr = (char*)(s.c_str());
    if((ptr = strchr(ptr, ',')) != NULL) h = atof(ptr + 1);
  }
  if(h < 1.0) h = 1.0;
  return h;
}

/*
 *
 */
float
PSObject::paper_width() {
  float w = -1.0;
  string s = parameters["PageSize"];
  if(s == "a0") w = A0_WIDTH;
  if(s == "a1") w = A1_WIDTH;
  if(s == "a2") w = A2_WIDTH;
  if(s == "a3") w = A3_WIDTH;
  if(s == "a4") w = A4_WIDTH;
  if(s == "a5") w = A5_WIDTH;
  if(s == "letter") w = LETTER_WIDTH;
  if(w < 0.0) w = atof(s.c_str());
  if(w < 1.0) w = 1.0;
  return w;
}

/*
 *
 */
unsigned long
PSObject::size() const {
  if(output_size == 0) return file_size;
  return output_size;
}

/*
 *
 */
float
PSObject::width() {
  if(parameters["PageOrientation"] == "landscape")
    return paper_height();
  return paper_width();
}

/*
 *
 */
static string
pacify(string s) {
  char c = '\0';
  unsigned int i;
  unsigned int len = s.length();
  for(i = 0; i < len; i++) {
    c = s[i];
    if(!isprint(c)) c = '?';
    if(strchr("()\\", c) != NULL) c = ' ';
    s[i] = c;
  } 
  return s;
}

