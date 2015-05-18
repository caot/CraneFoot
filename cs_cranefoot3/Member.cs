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
	/// Summary description for Member.
	/// </summary>
	internal class Member
	{
		private string m_name;

		public bool is_original = false;
		public char gender = '\0';
		public int father = -1;
		public int mother = -1;
		public int child = -1;
		public int sibling = -1;
		public int parent = -1;
		public int tree = -1;
		public int vertex = -1;
		public float x = 0.0f;
		public float y = 0.0f;
		public float age = 0.0f;
		public float height = 1.0f;
		public float width = 1.0f;
		public float up_attach = 0.0f;
		public ArrayList bonds = new ArrayList();

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Member name.</param>
		public Member(string name)
		{
			this.m_name = name;
		}

		/// <summary>
		/// Gets the member name.
		/// </summary>
		public string Name { get { return this.m_name; } }
	}
}
