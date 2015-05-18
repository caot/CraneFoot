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
	/// Compare two families by family name.
	/// </summary>
	internal class CompareFamily : IComparer
	{
		/// <summary>
		/// Stores the list of vertices (instances of Vertex).
		/// </summary>
		private ArrayList graph;

		public CompareFamily(ArrayList graph)
		{
			this.graph = graph;
		}

		public int Compare(object x, object y)
		{
			Vertex v1 = (Vertex)this.graph[(int)x];
			Vertex v2 = (Vertex)this.graph[(int)y];

			return v1.family_name.CompareTo(v2.family_name);
		}
	}

	/// <summary>
	/// Compare two emtries by father and mother id.
	/// </summary>
	internal class CompareParent : IComparer
	{
		public int Compare(object x, object y)
		{
			MEntry me1 = (MEntry)x;
			MEntry me2 = (MEntry)y;

			if(me1.father < me2.father) 
				return -1;
			
			if(me1.mother < me2.mother) 
				return -1;
			
			if (me1.father == me2.father && me1.mother < me2.mother)
				return 0;
			else
				return 1;
		}
	}

	/// <summary>
	/// Compare two entries by age.
	/// </summary>
	internal class CompareAge : IComparer
	{
		public int Compare(object x, object y)
		{
			MEntry me1 = (MEntry)x;
			MEntry me2 = (MEntry)y;

			return (me1.age.CompareTo(me2.age));
		}
	}
}
