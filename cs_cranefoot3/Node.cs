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
	/// A mating unit (male-female pair). An individual can be present in multiple nodes.
	/// </summary>
	public struct Node 
	{
		/// <summary>
		/// Unique node identification.
		/// </summary>
		public int index;

		/// <summary>
		/// The branch where the node resides in the family graph.
		/// </summary>
		public int tree;

		/// <summary>
		/// Indicates if the two individuals that make the mating unit are present elsewhere in the graph as 
		/// blood-relatives. That is, if an individual is present in this mating as a blood-relative for the first
		/// time.
		/// </summary>
		public bool origin_a;
		/// <summary>
		/// Indicates if the two individuals that make the mating unit are present elsewhere in the graph as 
		/// blood-relatives. That is, if an individual is present in this mating as a blood-relative for the first
		/// time.
		/// </summary>
		public bool origin_b;

		//
		// Gengers. 'M' denotes male, 'F' denotes female.
		//

		/// <summary>
		/// Mating unit first (a) member gender.
		/// </summary>
		public char gender_a;
		/// <summary>
		/// Mating unit second (b) member gender.
		/// </summary>
		public char gender_b;

		//
		// Positions of the individuals. 
		//

		/// <summary>
		/// Mating unit first (a) member x position.
		/// </summary>
		public float x_a;
		/// <summary>
		/// Mating unit second (b) member x position.
		/// </summary>
		public float x_b;
		/// <summary>
		/// Mating unit y position.
		/// </summary>
		public float y;

		//
		// Names of the individuals. 
		//

		/// <summary>
		/// Mating unit first (a) member name.
		/// </summary>
		public string alpha;
		/// <summary>
		/// Mating unit second (b) member name.
		/// </summary>
		public string beta;

		/// <summary>
		/// Children of the mating unit, indicated by node indices.
		/// </summary>
		public ArrayList children;

		/// <summary>
		/// Links to mating units that share one of the individuals, indicated by node indices.
		/// </summary>
		public ArrayList links;
	}
}