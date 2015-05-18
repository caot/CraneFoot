/* file: cranefoot.h
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

#ifndef cranefoot_INCLUDED
#define cranefoot_INCLUDED

#include <map>
#include <string>
#include <vector>

namespace cranefoot {

  /*
   * Pedigree record. Each individual must have exactly one vertex.
   */
  struct Vertex {

    /* Unique name for each vertex. */
    std::string name;

    /* Unique name for each family (i.e. distinct set of vertices). */
    std::string family_name;

    /* Name of the father vertex. If no such vertex exists, father is
       considered missing. */
    std::string father;

    /* Name of the mother vertex. If no such vertex exists, mother is
       considered missing. */
    std::string mother;

    /* Node ordering in the family graph. The order of siblings is determined
       by age so that those with high age are given small x-coordinates. */
    float age;

    /* Horizontal space requirement on the canvas. */
    float width;

    /* Vertical space requirement on the canvas. */
    float height;

    /* Horizontal position to which an upward link is attached in the
       family drawing. Must be non-negative and less than vertex width. */
    float up_attach;
  };

  /*
   * Family graph record that represent a mating unit. An individual can be
   * present in multiple nodes.
   */
  struct Node {

    /* Unique node identification. */
    int index;

    /* The branch where the node resides in the family graph. */
    int tree;

    /* Indicates if the two individuals that make the mating unit are
       present elsewhere in the graph as blood-relatives. That is, if an
       individual is present in this mating as a blood-relative for the first
       time. */
    bool origin_a;
    bool origin_b;

    /* Gengers. 'M' denotes male, 'F' denotes female. */
    char gender_a;
    char gender_b;

    /* Positions of the individuals. */
    float x_a;
    float x_b;
    float y;

    /* Names of the individuals. */
    std::string alpha;
    std::string beta;

    /* Children of the mating unit, indicated by node indices. */
    std::vector<int> children;

    /* Links to mating units that share one of the individuals, indicated
       by node indices.*/
    std::vector<int> links;
  };

  /*
   * Topological formulation of the pedigree and its graph representation
   * on the canvas.
   */
  class Family {
  private:
    void* buffer;
  public:

    /* Create empty family. */
    Family();

    /* Copy constructor. Creates a full duplicate. */
    Family(const Family&);

    /* Create a new family object from a list of pedigree records. The
       topology is determined by child-parent links. */
    Family(const std::vector<Vertex>&);

    /* Free all resources associated with the family. */
    ~Family();

    /* Copy contents of the argument to the calling object.
       The old contents are discarded. */
    void operator=(const Family&);

    /* List of topological errors. */
    std::vector<std::string> errors();

    /* Height of the family graph on the canvas. This is not constant, i.e. it
       might change when the layout is changed by additional simulations. */
    float height();

    /* Check if family is non-empty and topologically correct. */
    bool is_consistent();

    /* Family name. */
    std::string name();

    /* Family graph layout. */
    std::vector<Node> nodes();

    /* Apply a simulated annealing algorithm to position the family branches
       on the canvas. The first argument specifies the desired amount of
       time to be consumed. The second argument is the seed for the
       pseudo-number generator (ignored if non-positive). The third argument
       indicates whether runtime messages should be printed on the screen.*/
    unsigned int simulate(const float, const int, const bool);

    /* Number of family members. */
    unsigned int size();

    /* Width of the family graph on the canvas. This is not constant, i.e. it
       might change when the layout is changed by additional simulations. */
    float width();

    /* Create families from pedigree records. For each unique family name,
       one family object is created. */
    static std::map<std::string, Family> create(const std::vector<Vertex>&);

    /* Version identification. */
    static std::string version();
  };

  /*
   * User interface and miscellaneous program logic.
   */
  class Pedigree {
  private:
    void* buffer;
  public:

    /* Create an empty pedigree. */
    Pedigree();

    /* Copy constructor. This is added only for implicit use and should not
       be called explicitly. It does not create a true copy but transfers
       the data structures from the argument to the returned object. */
    Pedigree(const Pedigree&);

    /* Create a new pedigree as instructed in the configuration file.
       The argument specificies the path to the file. */
    Pedigree(const std::string&, const std::string&);

    /* Free all resources associated with the pedigree. */
    ~Pedigree();

    /* Swap the contents of the argument and the calling object. Note that
       this does not correspond to duplicating the argument. */
    void operator=(const Pedigree&);

    /* Return a new family object with the specified name. If none is found,
       an empty family is returned. */
    Family operator[](const std::string&);

    /* Return all family objects. */
    std::map<std::string, Family> families();

    /* Pedigree name. */
    std::string name();

    /* Position individuals on the canvas and write results as instructed in
       the configuration file. */
    bool run();

    /* Number of individuals. */
    unsigned int size();

    /* Version identification. */
    static std::string version();
  };
};

#endif /* cranefoot_INCLUDED */
