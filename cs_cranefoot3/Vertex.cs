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

namespace PedigreeDraw
{
	/// <summary>
	/// Pedigree record. Each individual must have exactly one vertex.
	/// </summary>
	public struct Vertex 
	{
		/// <summary>
		/// Unique name for each vertex.
		/// </summary>
		public string name;

		/// <summary>
		/// Unique name for each family (i.e. distinct set of vertices).
		/// </summary>
		public string family_name;

		/// <summary>
		/// Name of the father vertex. If no such vertex exists, father is considered missing.
		/// </summary>
		public string father;

		/// <summary>
		/// Name of the mother vertex. If no such vertex exists, mother is considered missing.
		/// </summary>
		public string mother;

		/// <summary>
		/// Node ordering in the family graph. The order of siblings is determined by age so that those 
		/// with high age are given small x-coordinates.
		/// </summary>
		public float age;

		/// <summary>
		/// Horizontal space requirement on the canvas.
		/// </summary>
		public float width;

		/// <summary>
		/// Vertical space requirement on the canvas. 
		/// </summary>
		public float height;

		/// <summary>
		/// Horizontal position to which an upward link is attached in the family drawing. Must be 
		/// non-negative and less than vertex width. 
		/// </summary>
		public float up_attach;

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Individual Id.</param>
		/// <param name="family_name">Family Id.</param>
		/// <param name="father">Father Id.</param>
		/// <param name="mother">Mother Id.</param>
		public Vertex (string name, string family_name, string father, string mother)
		{
			this.name = name;
			this.family_name = family_name;
			this.father = father;
			this.mother = mother;

			this.age = 0.0f;
			this.width = 2.0f;
			this.height = 2.0f;
			this.up_attach = 1.0f;
		}
	}
}