# CraneFoot v3 API

The source code is organized in a few independent modules that communicate
through streamlined object interfaces. For many classes there is one
simplified definition that calls a more complicated hidden object. This is to
avoid cluttering the header files with unnecessary member declarations.

The naming of the files reveals their purpose. For example, 'cranefoot.h'
contains the simplified class definition for 'Pedigree', but the actual class
'PedigreeObject' that does most of the work is defined in 'pedigreeobject.h'. 
A member method is designated by dot, e.g. 'pedigreeobject.configure.cpp'
contains the implementation of 'configure' method of 'PedigreeObject'. 
Sometimes it is convenient to collect several methods to one file, denoted by
'_methods.cpp' suffix. Similarly, non-member functions are denoted by
'_functions.cpp'.

Below are the important header files that are documented:

'cranefoot.h'
namespaces:    cranefoot
structs:       Vertex, Node
classes:       Family, Pedigree

'tablet.h'
namespaces:    tablet
structs:
classes:       Row, Table, ConfigTable

'scriptum.h'
namespaces:    scriptum
structs:
classes:       PostScript

Overall, the code should be legible, although the documentation is mostly on
the micromanagerial level. If anyone is enthusiastic enough to modify, improve
or clarify the software structure, I would be most happy to hear from it.

PS. Note also the C#-interface (cs_cranefoot2.zip) by A. Gurau.

-- Ville Makinen

# Linux/UNIX installation
Before you can start using CraneFoot on other than Windows systems, you have
to unpack the source code and combile it to a binary executable. In most UNIX-
based systems the compiler software is already installed. CraneFoot is written in
C/C++ and usual commands for a C++ compiler include c++ and g++, the latter
is associated with the GNU compiler which is usually a safe choice. To make the
executable, unpack the installation package (unzip or similar), go to the ’source’
directory and type

`g++ -O5 -o cranefoot main.cc *.cpp -lm`


# java interface
The java interface is at link https://github.com/caot/CraneFootJava
