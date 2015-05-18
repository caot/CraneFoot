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
	/// Class representing a vertex used by the Walker's algorithm.
	/// </summary>
	public class walker_vertex_t
	{
		/* Relations. Negative values are considered missing links. */

		/// <summary>
		/// Parent vertex id.
		/// </summary>
		public int parent;
		/// <summary>
		/// child vertex id.
		/// </summary>
		public int child;
		/// <summary>
		/// Sibling vertex id.
		/// </summary>
		public int sibling;

		/// <summary>
		/// A positive value indicates that the vertex needs to be adjusted half way in the horizontal space 
		/// available for it and its subtree. Otherwise it is put as close to the left neighboring subtree as possible.
		/// </summary>
		public int is_centering;

		/* These will be over-written by the node-positioning function. */

		/// <summary>
		/// Vertex x position.
		/// </summary>
		public float x;
		/// <summary>
		/// Vertex y position.
		/// </summary>
		public float y;

		/// <summary>
		/// Vertex horizontal space requirement.
		/// </summary>
		public float width;
		/// <summary>
		/// Vertex vertical space requirement.
		/// </summary>
		public float height;

		/// <summary>
		/// Attachment to parent vertex in the tree.
		/// </summary>
		public float up_attach;
		/// <summary>
		/// Attachement to child vertex in the tree.
		/// </summary>
		public float down_attach;

		/// <summary>
		/// Not used.
		/// </summary>
		public object user_data;
	}

	/// <summary>
	/// Class representing a node in the tree.
	/// </summary>
	public class node_t
	{
		public int parent;
		public int first_child;
		public int last_child;
		public int sibling;
		public int left_peer;
		public int right_peer;
		public int contour;
		public int gen;
		public int n_left_sibs;
		public float x;
		public float xmod;
		public float xshift;
		public float xchange;
		public float width;
	}

	/// <summary>
	/// Class representing a tree.
	/// </summary>
	public class tree_t
	{
		public int n;
		public int cap;
		public int gen;
		public int depth;
		public float modifier;
		public int [] vtx2node;
		public int [] node2vtx;
		public walker_vertex_t [] vertices;
		public node_t [] nodes;
		public node_t [] edge;
	}

	/// <summary>
	/// Structure reresenting a bond between two nodes.
	/// </summary>
	public struct Bond
	{
		public int a;
		public int b;

		public Bond(int a, int b)
		{
			this.a = a;
			this.b = b;
		}
	}

	public struct Argument 
	{
		public int tree;
		public int level;
		public int depth;
		public ArrayList vertices;			// Array of integers.
		public ArrayList members;			// Array of Member.
	}

	public struct MEntry 
	{
		public int index;
		public int father;
		public int mother;
		public float age;
	}
}