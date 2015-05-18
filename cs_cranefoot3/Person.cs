/*
  C# interface for CraneFoot
  Copyright (C) 2006 Alexandru Gurau

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

  CONTACT
    Alexandru Gurau, M. Ing.
    Biostatistician
    CHUM Research Center
    2901, rue Rachel est
    Montréal (Québec) H1W 4A4
    CANADA
    Tel: (514) 890-8000 poste 23651
    Email: alexandru.gurau.chum@ssss.gouv.qc.ca
*/
using System;
using System.Collections;

namespace PedigreeDraw
{
	/// <summary>
	/// Class representing all the information associated with an individual which we want to draw on 
	/// a pedigree chart.
	/// </summary>
	public class Person
	{
		/// <summary>
		/// Unique name for each individual.
		/// </summary>
		private string name;

		/// <summary>
		/// Unique name for each family (i.e. distinct set of individuals).
		/// </summary>
		private string family;

		/// <summary>
		/// Name of the individual's father. If no such vertex exists, father is considered missing.
		/// </summary>
		private string father;

		/// <summary>
		/// Name of the individual's mother. If no such vertex exists, mother is considered missing.
		/// </summary>
		private string mother;

		/// <summary>
		/// Individual gender: 'M' for males and 'F' for females.
		/// </summary>
		private char gender;

		/// <summary>
		/// Individual's age. Used to sort the individuals in age decreasing order form left to right.
		/// </summary>
		private int age = -1;

		/// <summary>
		/// True if the individual is dead, false otherwise. Use the SLASH variable.
		/// </summary>
		private bool slash = false;

		/// <summary>
		/// True if the individual is SNPed, false otherwise. Use the ARROW variable.
		/// </summary>
		private bool arrow = false;

		/// <summary>
		/// Additional information to be displayed with the individual.
		/// </summary>
		private ArrayList text = new ArrayList();

		/// <summary>
		/// A list of colors to fill in the indvidual associated shape to represent affections.
		/// </summary>
		private ArrayList colors = new ArrayList();

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Individual's name.</param>
		/// <param name="family">Individual's family name.</param>
		/// <param name="father">Individual's father.</param>
		/// <param name="mother">Individual's mother.</param>
		/// <param name="gender">Individual's gender (M/F).</param>
		public Person(string name, string family, string father, string mother, char gender)
		{
			this.name = name;
			this.family = family;
			this.father = father;
			this.mother = mother;
			this.gender = gender;
		}

		/// <summary>
		/// Gets the individual's name.
		/// </summary>
		public string Name { get { return this.name; } }
		/// <summary>
		/// Gets the family name.
		/// </summary>
		public string Family { get { return this.family; } }
		/// <summary>
		/// Gets the individual's father name.
		/// </summary>
		public string Father { get { return this.father; } }
		/// <summary>
		/// Gets the individual's mother name.
		/// </summary>
		public string Mother { get { return this.mother; } }
		/// <summary>
		/// Gets the individual's gender.
		/// </summary>
		public char Gender { get { return this.gender; } }

		/// <summary>
		/// Gets the individual gender associated shape.
		/// </summary>
		public Shapes Shape 
		{ 
			get 
			{
				if (this.gender == 'm')
					return Shapes.Square;
				else if (this.gender == 'f')
					return Shapes.Circle;
				else
					return Shapes.Diamond;
			}
		}

		/// <summary>
		/// Gets / sets the individual's age.
		/// </summary>
		public int Age
		{
			get { return this.age; }
			set { this.age = value; }
		}
		/// <summary>
		/// Gets / sets the individual's slash feature.
		/// </summary>
		public bool SlashFeature
		{
			get { return this.slash; }
			set { this.slash = value; }
		}
		/// <summary>
		/// Gets / sets the individual's arrow feature.
		/// </summary>
		public bool ArrowFeature
		{
			get { return this.arrow; }
			set { this.arrow = value; }
		}
		/// <summary>
		/// Gets / sets the individual's additional information text.
		/// </summary>
		public ArrayList Text
		{
			get { return this.text; }
			set { this.text = value; }
		}
		/// <summary>
		/// Gets / sets the individual's affection color codes.
		/// </summary>
		public ArrayList Colors
		{
			get { return this.colors; }
			set { this.colors = value; }
		}
	}
}
