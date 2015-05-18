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
	/// Structure holding an individual's symbol associated information.
	/// </summary>
	public class Emblem 
	{
		/// <summary>
		/// Used to represent the gender: square for males, circle for females, diamond for unknown.
		/// </summary>
		private Shapes shape;

		/// <summary>
		/// Used for dead individuals.
		/// </summary>
		private bool slash;

		/// <summary>
		/// Used to tag some individual's special feature like SNPed or propositus etc.
		/// </summary>
		private bool arrow;

		/// <summary>
		/// Used to represent different affection status.
		/// </summary>
		private ArrayList colors = new ArrayList();

		/// <summary>
		/// Used for additional information.
		/// </summary>
		private ArrayList text = new ArrayList();

		/// <summary>
		/// Individual age.
		/// </summary>
		private string age;

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="shape">The simbol shape - one of the enumeration Shapes value.</param>
		/// <param name="text">The lines of text to be shown beneath the symbol.</param>
		public Emblem(Shapes shape, ArrayList text)
		{
			this.shape = shape;
			this.text = text;
			this.slash = false;
			this.arrow = false;
		}

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="shape">The simbol shape - one of the enumeration Shapes value.</param>
		/// <param name="text">The lines of text to be shown beneath the symbol.</param>
		/// <param name="slash">True to show the slash over the symbol.</param>
		/// <param name="arrow">True to show the arrow next to the symbol.</param>
		/// <param name="colors">The list of color names representing affections.</param>
		public Emblem(Shapes shape, ArrayList text, bool slash, bool arrow, ArrayList colors)
		{
			this.shape = shape;
			this.text = text;
			this.slash = slash;
			this.arrow = arrow;
			this.colors = colors;
		}

		/// <summary>
		/// Gets the symbol shape.
		/// </summary>
		public Shapes Shape { get { return this.shape; } }
		/// <summary>
		/// Gets the symbol associated text lines.
		/// </summary>
		public ArrayList Text { get { return this.text; } }
		/// <summary>
		/// Gets the symbol slash feature.
		/// </summary>
		public bool SlashFeature { get { return this.slash; } }
		/// <summary>
		/// Gets the symbol arrow feature.
		/// </summary>
		public bool ArrowFeature { get { return this.arrow; } }
		/// <summary>
		/// Gets the symbol colors set.
		/// </summary>
		public ArrayList Colors { get { return this.colors; } }

		/// <summary>
		/// Gets / sets the person's age.
		/// </summary>
		public string Age 
		{
			get { return this.age; }
			set { this.age = value; }
		}
	}

	/// <summary>
	/// Enumeration of symbol available shapes.
	/// </summary>
	public enum Shapes
	{
		/// <summary>
		/// Square shape used to represent males.
		/// </summary>
		Square,
		/// <summary>
		/// Circle shape used to represent females.
		/// </summary>
		Circle,
		/// <summary>
		/// Diamond shape used to represent unknown gender.
		/// </summary>
		Diamond
	}
}