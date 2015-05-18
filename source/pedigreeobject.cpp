/* file: pedigreeobject.cpp
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

static bool check_parameters(ConfigTable&);
static void print_greeting(int);
static void print_checklist();

/*
 *
 */
PedigreeObject::PedigreeObject() {
  print_greeting(1);
}

/*
 *
 */
PedigreeObject::PedigreeObject(const string& fname, const string& pedigreeFile) {
  /* Check configuration file. */
  cfg = ConfigTable(fname, '\0');
  if (!pedigreeFile.empty())
    cfg.setPedigreeFilename(pedigreeFile);
  if(cfg.size() < 1) {
    cout << "WARNING! Configuration file '" << cfg.source()
	 << "' is empty or cannot be opened.\n";
    return;
  }

  /* Load data from input file(s). */
  verbose_mode = (cfg["VerboseMode"][1] != "off");
  if(verbose_mode) print_greeting(0);
  if(!check_parameters(cfg)) {
    cerr << "ERROR! Erroneous instructions found.\n";
    print_checklist();
    return;
  }  
  if(!configure()) {
    cerr << "ERROR! Configuration failed.\n";
    print_checklist();
    return;
  }
  if(!import()) {
    cerr << "ERROR! Import failed.\n";
    print_checklist();
    return;
  }
}

/*
 *
 */
static bool
check_parameters(ConfigTable& cfg) {
  bool flag = true;

  /* Check obsolite parameters. */
  if(cfg["EPSBase"].size() > 0) {
    cout << "WARNING! \"EPSBase\" is obsolite, "
	 << "use \"PedigreeName\" instead.\n";
    flag = false;
  }
  if(cfg["FamilyVariable"].size() > 0) {
    cout << "WARNING! \"FamilyVariable\" is obsolite, "
	 << "use \"SubgraphVariable\" instead.\n";
    flag = false;
  }
  if(cfg["LabelVariable"].size() > 0) {
    cout << "WARNING! \"LabelVariable\" is obsolite, "
	 << "use \"TextVariable\" instead.\n";
    flag = false;
  }
  if(cfg["SibCheck"].size() > 0) {
    cout << "WARNING! \"SibCheck\" is obsolite.\n";
    flag = false;
  }
  if(cfg["PSFile"].size() > 0) {
    cout << "WARNING! \"PSFile\" is obsolite, "
	 << "use \"PedigreeName\" instead.\n";
    flag = false;
  }
  if(cfg["PaperSize"].size() > 0) {
    cout << "WARNING! \"PaperSize\" is obsolite, "
	 << "use \"PageSize\" instead.\n";
    flag = false;
  }
  if(cfg["QuietRun"].size() > 0) {
    cout << "WARNING! \"QuietRun\" is obsolite, "
	 << "use \"VerboseMode\" instead.\n";
    flag = false;
  }
  if(cfg["Separator"].size() > 0) {
    cout << "WARNING! \"Separator\" is obsolite, "
	 << "use \"Delimiter\" instead.\n";
    flag = false;
  }
  if(cfg["TXTFile"].size() > 0) {
    cout << "WARNING! \"TXTFile\" is obsolite, "
	 << "use \"PedigreeName\" instead.\n";
    flag = false;
  }

  /* Check critical parameters. */
  if(cfg.getPedigreeFilename().size() < 2) {
    cout << "WARNING! Pedigree file not defined.\n";
    flag = false;
  }
  if(cfg.getPedigreeName().size() < 2) {
    cout << "WARNING! Pedigree name not defined.\n";
    flag = false;
  }
  if(cfg["NameVariable"].size() < 2) {
    cout << "WARNING! Name variable not defined.\n";
    flag = false;
  }
  if(cfg["FatherVariable"].size() < 2) {
    cout << "WARNING! Father variable not defined.\n";
    flag = false;
  }
  if(cfg["MotherVariable"].size() < 2) {
    cout << "WARNING! Mother variable not defined.\n";
    flag = false;
  }

  /* Check that shape and gender data. */
  if((cfg["GenderVariable"].size() > 1) &&
     (cfg["ShapeVariable"].size() > 1)) {
    cout << "WARNING! Both shape and gender variables detected.\n";
    cout << "         Please choose only one of the two.\n";
    flag = false;
  }
  if(cfg["MotherVariable"].size() < 2) {
    cout << "WARNING! Mother variable not defined.\n";
    flag = false;
  }

  return flag;
}

/*
 *
 */
static void
print_greeting(int level) {
  cout << Pedigree::version() << "\tPedigree\n";
  cout << Family::version() << "\tFamily\n";
  cout << PostScript::version() << "\tPostScript\n";
  cout << Table::version() << "\tTable\n";

  cout << "\nCraneFoot, copyright (C) 2006 Ville-Petteri Makinen\n";
  cout << "CraneFoot comes with ABSOLUTELY NO WARRANTY.\n";
  cout << "This is free software, and you are welcome to redistribute it\n";
  cout << "under certain conditions; see the license file for details.\n";

  if(level < 1) return;

  cout << "\n  Usage: cranefoot CFGFILE";
  cout << "\n         cranefoot CFGFILE PedigreeFile\n\n";

  cout << "  Format of CFGFILE:\n";
  cout << "  PARAM                  VALUE1      VALUE2\n";
  cout << "  # ... comment line ...\n";
  cout << "\n";

  cout << "  # File instructions:\n";
  cout << "  PedigreeFile           (string)\n";
  cout << "  PedigreeName           (string)\n";
  cout << "\n";

  cout << "  # Structural instructions:\n";
  cout << "  SubgraphVariable       (string)\n";
  cout << "  NameVariable           (string)\n";
  cout << "  FatherVariable         (string)\n";
  cout << "  MotherVariable         (string)\n";
  cout << "  GenderVariable         (string)\n";
  cout << "  AgeVariable            (string)\n";
  cout << "\n";

  cout << "  # Visualization instructions:\n";
  cout << "  ArrowVariable          (string)\n";
  cout << "  ColorVariable          (string)\n";
  cout << "  PatternVariable        (string)\n";
  cout << "  ShapeVariable          (string)\n";
  cout << "  SlashVariable          (string)\n";
  cout << "  TextVariable           (string)    # multiple\n";
  cout << "  TracerVariable         (string)\n";
  cout << "  #                      label       code\n";
  cout << "  ColorInfo              (string)    (integer)  # multiple\n";
  cout << "  PatternInfo            (string)    (integer)  # multiple\n";
  cout << "  ShapeInfo              (string)    (integer)  # multiple\n";
  cout << "\n";

  cout << "  # Formatting and functional instructions:\n";
  cout << "  BackgroundColor        (integer)\n";
  cout << "  Delimiter              tab/ws/(character)\n";
  cout << "  FigureLimit            (integer)\n";
  cout << "  FontSize               (real)\n";
  cout << "  ForegroundColor        (integer)\n";
  cout << "  PageSize               a0...a5/letter  a0...a5/letter/auto\n";
  cout << "  PageOrientation        portrait/landscape\n";
  cout << "  RandomSeed             (integer)\n";
  cout << "  TimeLimit              (real)\n";
  cout << "  VerboseMode            on/off\n";
  cout << "\n";

  cout << "  Written by Ville-Petteri Makinen\n";
  cout << "  http://www.iki.fi/~vpmakine\n";
  cout << "\n";
}

/*
 *
 */
static void
print_checklist() {
  cout << "\nAn error occurred, so please check that\n";
  cout << "  1. Input files are accessible.\n";
  cout << "  2. Input files are not empty.\n";
  cout << "  3. Input files are plain text, not e.g. MS Office documents.\n";
  cout << "  4. You are using the correct delimiter (default is tab).\n";
  cout << "  5. The file names are configured correctly.\n";
  cout << "  6. The pedigree variables are configured correctly.\n";
  cout << "  7. Every data file contains the name variable.\n";
  cout << "  8. Every instruction in the configuration file is unique.\n";
  cout << "  9. Variable names in the input files are unique.\n";
}
