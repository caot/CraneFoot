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
using System.Text;
using System.Collections;

namespace PedigreeDraw
{
	/// <summary>
	/// Class representing a family generated from a set of individuals (members).
	/// </summary>
	public class Family
	{
		/// <summary>
		/// Male.
		/// </summary>
		public const char MALE = 'M';
		/// <summary>
		/// Female.
		/// </summary>
		public const char FEMALE = 'F';
		/// <summary>
		/// Upper random number generator limit.
		/// </summary>
		public const int RAND_MAX = 50000;
		/// <summary>
		/// Random generator seed.
		/// </summary>
		public const int RANDOM_SEED = 186;
		/// <summary>
		/// Number of ticks per second.
		/// </summary>
		public const int CLOCKS_PER_SEC = 1000;
		/// <summary>
		/// Maximum recursion depth.
		/// </summary>
		public const int  MAX_RECURSION_DEPTH = 5000;

		/// <summary>
		/// Family chart height.
		/// </summary>
		private float chartHeight;
		/// <summary>
		/// Family chartwidth.
		/// </summary>
		private float chartWidth;
		/// <summary>
		/// Family name.
		/// </summary>
		private string familyName;
		/// <summary>
		/// Family associated error list (string).
		/// </summary>
		private Queue familyErrorList;

		/// <summary>
		/// Family members - instances of class Member.
		/// </summary>
		internal ArrayList members;
		/// <summary>
		/// Family branches - instances of class Branch.
		/// </summary>
		internal ArrayList branches;

		/// <summary>
		/// Creates a family object from a list of vertices.
		/// </summary>
		/// <param name="vertices">The list of vertices - instances of structure Vertex.</param>
		public Family(ArrayList vertices) 
		{
			if(vertices.Count < 1) 
			{
				this.familyErrorList.Enqueue("Empty family.");
				return;
			}

			Hashtable name2index = new Hashtable();

			this.familyName = ((Vertex)vertices[0]).family_name;
			this.chartHeight = 0.0f;
			this.chartWidth = 0.0f;
			this.familyErrorList = new Queue(10);
			this.members = new ArrayList();
			this.branches = new ArrayList();

			for(int i = 0; i < vertices.Count; i++) 
			{
				Vertex currentVertex = (Vertex)vertices[i];

				if(currentVertex.family_name != this.familyName) 
				{
					this.members.Clear();
					Error("Multiple family names detected.");
					return;
				}

				if(currentVertex.name.Length < 1) 
				{
					this.members.Clear();
					Error("Nameless individual detected.");
					return;
				}

				if(name2index.ContainsKey(currentVertex.name)) 
				{
					this.members.Clear();
					Error("Duplicate individual '" + currentVertex.name + "' found.");
					return;
				}

				if(currentVertex.width <= 0.0) 
				{
					this.members.Clear();
					Error("Individual '" + currentVertex.name + "' has non-positive width.");
					return; 
				}

				if(currentVertex.height <= 0.0) 
				{
					this.members.Clear();
					Error("Individual '" + currentVertex.name + "' has non-positive height.");
					return; 
				}

				if(currentVertex.up_attach <= 0.0) 
				{
					this.members.Clear();
					Error("Individual '" + currentVertex.name + "' has non-positive up attach.");
					return; 
				}

				name2index.Add(currentVertex.name, i);
				this.members.Add(new Member(currentVertex.name));
			}

			//
			// Find parents and set dimensions. 
			//
			for(int i = 0; i < vertices.Count; i++) 
			{
				Vertex currentVertex = (Vertex)vertices[i];
				Member currentMember = (Member)this.members[i];
				
				// Set father index.
				if(name2index.ContainsKey(currentVertex.father))
					currentMember.father = (int)name2index[currentVertex.father];

				// Set mother index.
				if(name2index.ContainsKey(currentVertex.mother))
					currentMember.mother = (int)name2index[currentVertex.mother];

				currentMember.age = currentVertex.age;
				currentMember.height = currentVertex.height;
				currentMember.width = currentVertex.width;
				currentMember.up_attach = currentVertex.up_attach;
				currentMember.is_original = true;

				if(currentMember.up_attach > currentMember.width)
					currentMember.up_attach = currentMember.width;
			}

			Link(true);
			Branch();
		}

		/// <summary>
		/// Adds an error message to the error queue with a maximum capacity of 10.
		/// </summary>
		/// <param name="err">The error message.</param>
		private void Error(string err)
		{
			if(this.familyErrorList.Count > 9) 
				return;

			if(this.familyErrorList.Count < 9) 
				this.familyErrorList.Enqueue(err);
			else 
				this.familyErrorList.Enqueue("Remaining errors suppressed.");
		}

		/// <summary>
		/// Gets the associated error list.
		/// </summary>
		public string [] Errors 
		{ 
			get 
			{ 
				string [] errors = new string[this.familyErrorList.Count];
				this.familyErrorList.CopyTo(errors, 0);

				return errors;
			}
		}

		/// <summary>
		/// Gets the family height on the canvas.
		/// </summary>
		public float Height
		{
			get
			{
				if(this.branches.Count < 1) 
					return 0.0f;

				if(this.chartHeight <= 0.0f) 
					Update();

				return this.chartHeight;
			}
		}

		/// <summary>
		/// Gets the family width on the canvas.
		/// </summary>
		public float Width
		{
			get
			{
				if(this.branches.Count < 1) 
					return 0.0f;

				if(this.chartWidth <= 0.0f) 
					Update();

				return this.chartWidth;
			}
		}

		/// <summary>
		/// Gets the family status.
		/// </summary>
		public bool IsConsistent 
		{
			get
			{
				if(this.branches.Count < 1) 
					return false;
				if(this.familyErrorList.Count > 0) 
					return false;
				return true;
			}
		}

		/// <summary>
		/// Gets the family name.
		/// </summary>
		public string Name { get { return this.familyName; } }

		/// <summary>
		/// Gets the family size.
		/// </summary>
		public int Size { get { return this.members.Count; } }

		/// <summary>
		/// Create families from pedigree records. For each unique family name, one family object is created.
		/// </summary>
		/// <param name="vertices">The list of vertices.</param>
		/// <returns>The list of families.</returns>
		public static Hashtable Create(ArrayList vertices)
		{
			int k;
			string family_name = "";
			ArrayList view = new ArrayList();					// Store good vertices indexes.
			ArrayList subgraph = new ArrayList();
			Hashtable families = new Hashtable();

			if(vertices.Count < 1) 
				return families;

			//
			// Create a sorted view.
			//
			for(int i = 0; i < vertices.Count; i++) 
			{
				if(((Vertex)vertices[i]).name == "")			// Skip for missing vertex name.
					continue;
				if(((Vertex)vertices[i]).family_name == "")		// Skip for missing vertex family name.
					continue;

				view.Add(i);
			}

			//
			// Sort vertices by family names.
			//
			view.Sort(new CompareFamily(vertices));
  
			//
			// Create families.
			//
			for(int i = 0; i < view.Count; i++) 
			{
				k = (int)view[i];
				if(((Vertex)vertices[k]).name.Length < 1) 
					continue;

				if(family_name.Length < 1) 
					family_name = ((Vertex)vertices[k]).family_name;

				if(family_name != ((Vertex)vertices[k]).family_name)		// New family found.
				{
					families.Add(family_name, new Family(subgraph));		// Create the family
					subgraph.Clear();
					family_name = ((Vertex)vertices[k]).family_name;
				}

				subgraph.Add(vertices[k]);
			}

			if(subgraph.Count > 0)
				families.Add(family_name, new Family(subgraph));

			return families;
		}

		/// <summary>
		/// Gets the family associated list of nodes.
		/// </summary>
		public ArrayList Nodes
		{
			get
			{
				int ind, tree;

				ArrayList vertices = new ArrayList();		// Array of Node instances.
				ArrayList rank2index = new ArrayList();		// Array of integers.
				for (int i = 0; i < this.members.Count; i++)
					rank2index.Add(-1);

				//
				// Determine node height. 
				//
				float height = 0.0f;
				for(int i = 0; i < this.members.Count; i++) 
				{
					if((tree = ((Member)members[i]).tree) < 0) 
						continue;

					if(((Member)members[i]).height > height) 
						height = ((Member)members[i]).height;
				}

				//
				// Create nodes. 
				//
				Node node;
				for(int i = 0; i < members.Count; i++) 
				{
					if((tree = ((Member)members[i]).tree) < 0) 
						continue;

					node.tree = tree;
					node.origin_a = ((Member)members[i]).is_original;
					node.origin_b = false;
					node.gender_a = ((Member)members[i]).gender;
					node.gender_b = '\0';
					node.x_a = (((Branch)branches[tree]).x + ((Member)members[i]).x + 1.0f);
					node.x_b = node.x_a;
					node.y = (((Branch)branches[tree]).y + ((Member)members[i]).y + height);
					node.alpha = ((Member)members[i]).Name;
					node.beta = "";
					node.children = new ArrayList();
					node.links = new ArrayList();

					//
					// Non-leaf node - has children. 
					//
					if(((Member)members[i]).child >= 0) 
					{
						ind = ((Member)members[((Member)members[i]).child]).mother;
						if(ind == i) 
							ind = ((Member)members[((Member)members[i]).child]).father;

						node.gender_b = ((Member)members[ind]).gender;
						node.x_b += ((Member)members[i]).width;
						node.beta = ((Member)members[ind]).Name;
    
						//
						// Collect children. 
						//
						ind = ((Member)members[i]).child;
						while(ind >= 0) 
						{
							if(((Member)members[ind]).is_original)
								node.children.Add(ind);

							ind = ((Member)members[ind]).sibling;
						}
					}

					node.index = vertices.Count;
					rank2index[i] = node.index;
					vertices.Add(node);
				}

				//
				// Update addresses of children. 
				//
				for(int i = 0; i < vertices.Count; i++) 
				{
					ArrayList children = ((Node)vertices[i]).children;

					for(int k = 0; k < children.Count; k++)
						children[k] = (int)rank2index[(int)children[k]];
				}

				//
				// Collect original nodes.
				//
				Hashtable name2index = new Hashtable();			// Map of string and integers
				for(int i = 0; i < vertices.Count; i++) 
				{
					if(((Node)vertices[i]).origin_a == false) 
						continue;

					name2index[((Node)vertices[i]).alpha] = i;
				}

				//
				// Connect duplicates to originals. 
				//
				int l;
				for(int i = 0; i < vertices.Count; i++) 
				{
					string alpha = ((Node)vertices[i]).alpha; 
					string beta = ((Node)vertices[i]).beta;

					if(name2index.ContainsKey(alpha)) 
					{
						if((l = (int)name2index[alpha]) != i)
							((Node)vertices[l]).links.Add(i);
					}

					if(beta.Length < 1) 
						continue;

					if(name2index.ContainsKey(beta)) 
					{
						if((l = (int)name2index[beta]) != i)
							((Node)vertices[l]).links.Add(i);
					}
					else 
					{
						Node n = (Node)vertices[i];
						n.origin_b = true;
						name2index.Add(beta, i);
					}
				}

				return vertices;
			}
		}

		private void Update() 
		{
			float x, y;
			float [] box = new float[4] {float.MaxValue, float.MaxValue, float.MinValue, float.MinValue};

			if(this.branches.Count < 1)
				return;

			//
			// Determine bounding box.
			//
			for(int i = 0; i < this.branches.Count; i++) 
			{
				x = ((Branch)branches[i]).x;
				y = ((Branch)branches[i]).y;
				if(x < box[0]) 
					box[0] = x;
				if(y < box[1]) 
					box[1] = y;
				x += ((Branch)branches[i]).width;
				y += ((Branch)branches[i]).height;
				if(x > box[2]) 
					box[2] = x;
				if(y > box[3]) 
					box[3] = y;
			}

			//
			// Remove offsets.
			//
			for(int i = 0; i < this.branches.Count; i++) 
			{
				((Branch)branches[i]).x -= box[0];
				((Branch)branches[i]).y -= box[1];
			}

			this.chartWidth = (box[2] - box[0]);
			this.chartHeight = (box[3] - box[1] + 1.0f);
		}  

		/// <summary>
		/// Find full siblings, remove redundant child-parent links. Note the use of 'bonds' as a temporary child list.
		/// </summary>
		/// <param name="sibcheck"></param>
		/// <returns></returns>
		private int Link(bool sibcheck)
		{
			int n_nuclei = 0;
			int parent, coparent, stepparent;
			int sib, prevsib;

			//
			// Check topology and clear child lists.
			//
			if(!Check(sibcheck)) 
				return 0;

			for(int i = 0; i < this.members.Count; i++)
				((Member)members[i]).bonds.Clear();
  
			//
			// Link every child to descendant parent if possible. 
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				if((parent = ((Member)members[i]).mother) >= 0) 
				{
					if(((Member)members[parent]).mother >= 0) 
					{
						((Member)members[parent]).bonds.Add(i);	
						continue;
					}
				}

				if((coparent = ((Member)members[i]).father) >= 0) 
				{
					if(((Member)members[coparent]).mother >= 0) 
					{
						((Member)members[coparent]).bonds.Add(i);
						continue;
					}
				}

				if(parent >= 0) 
				{ 
					((Member)members[parent]).bonds.Add(i);
					continue;
				}

				if(coparent >= 0) 
					((Member)members[coparent]).bonds.Add(i);
			}
  
			//
			// Link siblings and remove non-first borns.
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				prevsib = -1;
				coparent = -1;
				SortSubset(((Member)members[i]).bonds, "parent");

				for(int k = 0; k < ((Member)members[i]).bonds.Count; k++) 
				{
					sib = (int)((Member)members[i]).bonds[k];
					((Member)members[sib]).sibling = -1;

					stepparent = ((Member)members[sib]).father;
					if(stepparent == i)
						stepparent = ((Member)members[sib]).mother;      

					if(stepparent == coparent) 
					{	
						if(prevsib < 0) 
							throw new PedigreeDrawException("ERROR! Illegal program state.");

						((Member)members[prevsib]).sibling = sib;
						prevsib = (int)((Member)members[i]).bonds[k];
						((Member)members[i]).bonds[k] = -1;					// Remove non-first born.
					}
					else 
					{
						coparent = stepparent;
						prevsib = sib;
						n_nuclei++;
					}
				}
			}

			//
			// Create duplicates to make family nuclei pure.
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				if(((Member)members[i]).bonds.Count < 1) 
					continue;

				SortSubset(((Member)members[i]).bonds, "parent");
				for(int k = 1; k < ((Member)members[i]).bonds.Count; k++) 
				{
					if((int)((Member)members[i]).bonds[k] < 0) 
						break;

					Member mb = new Member(((Member)members[i]).Name);
					mb.gender = ((Member)members[i]).gender;
					mb.father = ((Member)members[i]).father;
					mb.mother = ((Member)members[i]).mother;
					mb.sibling = ((Member)members[i]).sibling;
					mb.child = (int)((Member)members[i]).bonds[k];
					mb.age = ((Member)members[i]).age;
					mb.width = ((Member)members[i]).width;
					mb.height =((Member) members[i]).height;

					((Member)members[i]).sibling = this.members.Count;

					this.members.Add(mb);
				}

				((Member)members[i]).child = (int)((Member)members[i]).bonds[0];
				((Member)members[i]).bonds = new ArrayList();
			}

			//
			// Make sure links from children to parents are correct.
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				sib = ((Member)members[i]).child;

				if(((Member)members[i]).gender == MALE) 
				{
					while(sib >= 0) 
					{
						((Member)members[sib]).father = i;
						sib = ((Member)members[sib]).sibling;
					}
				}
				else 
				{
					while(sib >= 0) 
					{
						((Member)members[sib]).mother = i;
						sib = ((Member)members[sib]).sibling;
					}
				}
			}

			//
			// Sort siblings according to age. 
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				sib = ((Member)members[i]).child; 
				ArrayList sibs = new ArrayList();

				while(sib >= 0) 
				{
					sibs.Add(sib);
					sib = ((Member)members[sib]).sibling;
				}

				if(sibs.Count < 2) 
					continue;

				SortSubset(sibs, "age");

				((Member)members[i]).child = (int)sibs[0];
				for(int k = 1; k < sibs.Count; k++) 
				{
					((Member)members[(int)sibs[k-1]]).sibling = (int)sibs[k];
					((Member)members[(int)sibs[k]]).sibling = -1; 
				}
			}

			return n_nuclei;
		}

		/// <summary>
		/// Sort a subset of members by a given criteria.
		/// </summary>
		/// <param name="subset">The subset of members to be sorted.</param>
		/// <param name="mode">Sorting type: by parent or by age.</param>
		private void SortSubset(ArrayList subset, string mode) 
		{
			int n = subset.Count;
			if(n < 2) 
				return;

			ArrayList entries = new ArrayList();			// Array of MEntry structures. 

			//
			// Create entries. 
			//
			int ind;
			MEntry ent;
			for(int k = 0; k < n; k++) 
			{
				if((ind = (int)subset[k]) < 0)
					continue;

				ent.index = ind;
				ent.father = ((Member)this.members[ind]).father;
				ent.mother = ((Member)this.members[ind]).mother;
				ent.age = ((Member)this.members[ind]).age;

				entries.Add(ent);
			}

			//
			// Sort entries. 
			//
			if(mode == "parent")
				entries.Sort(new CompareParent());

			if(mode == "age")
				entries.Sort(new CompareAge());
			
			//
			// Update subset.
			//
			int l;
			for(l = 0; l < entries.Count; l++)
				subset[l] = ((MEntry)entries[l]).index;

			for(; l < n; l++)
				subset[l] = -1;
		}

		/// <summary>
		/// Create links "brances" between family nodes.
		/// </summary>
		/// <returns>The number of branches.</returns>
		private int Branch() 
		{
			int index;
			ArrayList v = new ArrayList();		// Array of integers.
			Bond bond = new Bond(-1, -1);
			Branch branch = new Branch(null);
			Argument arg;

			if(this.familyErrorList.Count > 0) 
				return 0;

			for(int i = 0; i < this.members.Count; i++) 
			{
				((Member)members[i]).x = -1.0f;
				((Member)members[i]).y = -1.0f;
				((Member)members[i]).bonds = new ArrayList();
			}

			//
			// Create bonds between coparents. 
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				if((index = ((Member)members[i]).child) < 0)		// No child.
					continue;

				bond = new Bond(((Member)members[index]).father, ((Member)members[index]).mother);

				if(bond.a < 0) 
					continue;

				if(bond.b < 0) 
					continue;

				((Member)members[bond.a]).bonds.Add(bond.b);
				((Member)members[bond.b]).bonds.Add(bond.a);    
			}

			//
			// Create bonds between originals and duplicates i.e. pseudo-siblings with the same name. 
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				if((index = ((Member)members[i]).sibling) < 0)		// No siblings.
					continue;

				if(((Member)members[i]).Name != ((Member)members[index]).Name) 
					continue;

				bond = new Bond(i, index);
				((Member)members[bond.a]).bonds.Add(bond.b);
				((Member)members[bond.b]).bonds.Add(bond.a);    
			}

			//
			// Copy topology from members.
			//
			arg.tree = 0;
			arg.members = this.members;
			for(int i = 0; i < this.members.Count; i++) 
			{
				Member currentMember = (Member)this.members[i];

				if(currentMember.father >= 0) 
					continue;
				if(currentMember.mother >= 0) 
					continue;
				if(currentMember.child < 0) 
					continue;
				if(currentMember.tree >= 0) 
					continue;

				branch = new Branch(this);
				arg.level = 0;
				arg.depth = 0;
				arg.vertices = branch.vertices;

				Dfs(arg, i, -1);

				branch.depth = arg.depth;
				this.branches.Add(branch);
				arg.tree += 1;
			}

			//
			// Create outsider branches. 
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				if(((Member)members[i]).tree >= 0) 
					continue;
				if(((Member)members[i]).bonds.Count > 0) 
					continue;

				branch = new Branch(this);
				((Member)members[i]).tree = arg.tree;
				((Member)members[i]).vertex = 0;
				branch.vertices.Add(i);
				this.branches.Add(branch);
				arg.tree += 1;
			}

			return this.branches.Count;
		}  

		private void Dfs(Argument arg, int i, int parent) 
		{
			if(i < 0) 
				return;

			int ind = -1;
			ArrayList vertices = arg.vertices;
			ArrayList members = arg.members;

			if(arg.level >= MAX_RECURSION_DEPTH) 
				throw new PedigreeDrawException("ERROR! Maximum recursion depth exceeded.");

			if(arg.level >= arg.depth)
				arg.depth = (arg.level + 1);

			//
			// Create vertex.
			//
			((Member)members[i]).x = vertices.Count;
			((Member)members[i]).y = arg.level;
			((Member)members[i]).parent = parent;
			((Member)members[i]).tree = arg.tree;
			((Member)members[i]).vertex = vertices.Count;
			
			vertices.Add(i);

			//
			// Descend to children.
			//
			if((ind = ((Member)members[i]).child) >= 0) 
			{
				arg.level += 1;
				Dfs(arg, ind, i);
				arg.level -= 1;
			}

			//
			// Cycle through siblings. 
			//
			while((i = ((Member)members[i]).sibling) >= 0) 
			{
				if(((Member)members[i]).tree >= 0) 
					break;

				Dfs(arg, i, parent);
			}
		}

		/// <summary>
		/// Check that topology is correct.
		/// </summary>
		/// <param name="sibcheck"></param>
		/// <returns>True if OK false otherwise.</returns>
		private bool Check(bool sibcheck) 
		{
			int n_children = 0;
			string name = "";
			ArrayList anom = new ArrayList();

			//
			// Check that topology is correct.
			//
			for(int i = 0; i < this.members.Count; i++) 
			{
				Member currentMember = (Member)members[i];

				if(currentMember.Name == name) 
					throw new PedigreeDrawException("ERROR! Illegal program state.");

				name = currentMember.Name;

				//
				// Either 0 or 2 parents. 
				//
				int father = currentMember.father;
				int mother = currentMember.mother;

				if((father >= 0) ^ (mother >= 0)) 
				{
					Error("'" + name + "'\tMust have either 0 or 2 parents.");
					continue;
				}

				if((father == mother) && (father >= 0)) 
				{
					Error("'" + name + "'\tParents must be distinct.");
					continue;
				}

				if(father < 0) 
					continue;

				n_children++;    

				//
				// A child cannot be his own parent.
				//
				if((father == i) || (mother == i)) 
				{
					Error("'" + name + "'\tChild-parent cycle.");
					continue;
				}
			    
				//
				// Father is male, mother is female. Set gender if unknown.
				//
				if(((Member)members[father]).gender == FEMALE) 
				{
					Error("'" + name + "'\tFather is already mother to other children.");
					continue;
				}
				((Member)members[father]).gender = MALE;

				if(((Member)members[mother]).gender == MALE) 
				{
			 		Error("'" + name + "'\tMother is already father to other children.");
					continue;
				}
				((Member)members[mother]).gender = FEMALE;
			    
				//
				// Check if sibling couple. 
				//
				int grandfather = ((Member)members[father]).father;
				int grandmother = ((Member)members[mother]).mother;

				if((grandfather >= 0) && (grandmother >= 0 )) 
				{
					if((grandfather == ((Member)members[mother]).father) ||
						(((Member)members[father]).mother == grandmother))
						anom.Add("'" + name + "'\tParents are siblings.");
				}

				//
				// Check if coupling within first-degree. 
				//
				if(father == ((Member)members[mother]).father)
					anom.Add("'" + name + "'\tGrandfather and father are the same.");

				if(mother == ((Member)members[father]).mother)
					anom.Add("'" + name + "'\tGrandmother and mother are the same.");
			}	// end for

			if((n_children < 1) && (familyErrorList.Count < 1))
				Error("empty\tNo offspring detected.");
			  
			//
			// List errors. 
			//
			StringBuilder sbMessage;
			sbMessage = new StringBuilder();
			if(this.familyErrorList.Count > 0) 
			{
				sbMessage.Append("WARNING! Errors in '" + this.familyName + "':\n");
				object error;
				try
				{
					while ((error = this.familyErrorList.Dequeue()) != null)
						sbMessage.Append("\t" + error.ToString() + "\n");
				}
				catch (System.InvalidOperationException) {}		// Empty queue.

				throw new PedigreeDrawException(sbMessage.ToString());
			}

			//
			// List anomalies. 
			//
			sbMessage = new StringBuilder();
			if(anom.Count > 0) 
			{
				sbMessage.Append("WARNING! Anomalies in '" + this.familyName + "':\n");
				for(int i = 0; i < anom.Count; i++) 
				{
					if(i >= 9) 
					{
						sbMessage.Append("\tRemaining anomalies suppressed.\n");
						break;
					}
					
					sbMessage.Append("\t" +  anom[i].ToString() + "\n");
				}

				throw new PedigreeDrawException(sbMessage.ToString());
			}

			return true;
		}


		/// <summary>
		/// Apply a simulated annealing algorithm to position the family branches on the canvas.
		/// </summary>
		/// <returns>The number of iterations.</returns>
		public int Simulate()
		{
			float limit = 10.0f;
			int seed = (int)DateTime.Now.Ticks;

			int n = 0;
			Random rnd = new Random(RANDOM_SEED);

			float rho = (float)Math.Sqrt(1.0 * this.members.Count * this.branches.Count);

			if(this.familyErrorList.Count > 0) 
				return 0;

			if(this.branches.Count < 1) 
				return 0;

			if(limit < 1e-2f) 
				return 0;

			//
			// Compute node positions inside branches.
			//
			for(int i = 0; i < this.branches.Count; i++) 
			{
				((Branch)this.branches[i]).Connect();
				((Branch)this.branches[i]).Walk();
			}

			//
			// Initial positions.
			//
			Update();
			for(int i = 0; i < this.branches.Count; i++) 
			{
				((Branch)this.branches[i]).x = rho * rnd.Next(RAND_MAX)/RAND_MAX;
				((Branch)this.branches[i]).y = rho * rnd.Next(RAND_MAX)/RAND_MAX;
			}

			//
			// Simulation parameters.
			//
			TimeSpan elapsed;
			DateTime alert = DateTime.Now;
			long start = DateTime.Now.Ticks;
			float dt = 0.0f;
			float temp0 = (float)Math.Log10(1.0 + rho);  
			float temp = temp0;

			//
			// Simulated annealing.
			//
			for(n = 0; temp > 0.05; n++) 
			{     
				// Check temperature. 
				elapsed = DateTime.Now - alert;
				if(Math.Abs(elapsed.TotalMilliseconds) > CLOCKS_PER_SEC/2.0) 
				{
					alert = DateTime.Now;

					if(n%2 != 0) 
						dt += 0.5f/limit;
					else 
						dt = (DateTime.Now.Ticks - start) * TimeSpan.TicksPerSecond/limit;

					float r = (float)Math.Exp(10*(dt - 0.6));
					if(seed <= 0) 
					{
						temp = (1.0f - r/(1.0f + r)) * temp0;
						if((dt > 0.95) && (temp > 0.05)) 
							temp = 0.05f;
					}  
				}

				// Update configuration. 
				Iterate(temp);
				temp *= (1.0f - 1.0f/(100.0f + members.Count));
			}

			// Eliminate unnecessary gaps. 
			Update();

			return n;
		}

		private void Iterate(float temp) 
		{
			float x, y, r;
			int n = this.branches.Count;
			if(n < 2) 
				return;  

			float [] box = new float [4] {float.MaxValue, float.MaxValue, float.MinValue, float.MinValue};
			float [] gx = new float [n];
			float [] gy = new float [n];
			ArrayList f;
			Random rnd = new Random(RANDOM_SEED);
	
			//
			// Determine geometric cost. 
			//
			for(int i = 0; i < n; i++) 
			{
				x = ((Branch)this.branches[i]).x;
				y = ((Branch)this.branches[i]).y;

				if(x < box[0]) 
					box[0] = x;
				if(y < box[1]) 
					box[1] = y;

				x += ((Branch)this.branches[i]).width;
				y += ((Branch)this.branches[i]).height;

				if(x > box[2]) 
					box[2] = x;
				if(y > box[3]) 
					box[3] = y;
			}

			x = (float)Math.Pow((box[2] - box[0]), 2);  
			y = (float)Math.Pow((box[3] - box[1]), 2);  
			r = 0.5f * (x + y + 1e-6f);
			x /= r;
			y /= r;

			//
			// Compute gradients. 
			//
			for(int i = 0; i < n; i++) 
			{
				for(int j = (i + 1); j < n; j++) 
				{
					f = ((Branch)this.branches[i]).Repel((Branch)this.branches[j]);

					gx[i] += (y + 0.2f * rnd.Next(RAND_MAX)/RAND_MAX) * (float)f[0];
					gy[i] += (x + 0.2f * rnd.Next(RAND_MAX)/RAND_MAX) * (float)f[1];
					gx[j] -= (y + 0.2f * rnd.Next(RAND_MAX)/RAND_MAX) * (float)f[0];
					gy[j] -= (x + 0.2f * rnd.Next(RAND_MAX)/RAND_MAX) * (float)f[1];
				}
				f = ((Branch)this.branches[i]).Attract();

				gx[i] += (0.8f + 0.2f * rnd.Next(RAND_MAX)/RAND_MAX) * (float)f[0];
				gy[i] += (0.8f + 0.2f * rnd.Next(RAND_MAX)/RAND_MAX) * (float)f[1];
			}

			//
			// Central attractor. 
			//
			float w = 0.5f * (box[2] - box[0] + 1e-6f);
			float h = 0.5f * (box[3] - box[1] + 1e-6f);
			float x0 = 0.5f * (box[2] + box[0]);
			float y0 = 0.5f * (box[3] + box[1]);
			r = (float)Math.Sqrt(w*w + h*h);

			for(int i = 0; i < n; i++) 
			{
				x = (x0 - ((Branch)this.branches[i]).x - 0.5f * (((Branch)this.branches[i]).width))/r;
				y = (y0 - ((Branch)this.branches[i]).y - 0.5f * (((Branch)this.branches[i]).height))/r;
				gx[i] += x;
				gy[i] += y;
			}

			//
			// Update positions. 
			//
			for(int i = 0; i < n; i++) 
			{
				r = (float)Math.Sqrt(gx[i]*gx[i] + gy[i]*gy[i]);
				if(r > temp) 
				{
					gx[i] *= temp/r;
					gy[i] *= temp/r;
				}

				((Branch)this.branches[i]).x += (gx[i] - box[0]);
				((Branch)this.branches[i]).y += (gy[i] - box[1]);
			}
		}
	}
}