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
	/// Summary description for Branch.
	/// </summary>
	internal class Branch
	{
		public const float REPULSION_FIELD = 0.5f;
		/// <summary>
		/// Random number generator upper limit.
		/// </summary>
		public const int RAND_MAX = 50000;
		/// <summary>
		/// Maximum number of vertices.
		/// </summary>
		public const int VERTEX_CAP = 1000000;
		/// <summary>
		/// Maximum recursion depth.
		/// </summary>
		public const int MAX_DEPTH = 10000;

		internal int depth;
		internal float x;
		internal float y;
		internal float width;
		internal float height;

		internal ArrayList bondings = new ArrayList();
		internal ArrayList vertices = new ArrayList();
		internal Family family;

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="fo">The family to be drawn.</param>
		internal Branch(Family fo)
		{
			this.family = fo;
			this.x = 0.0f;
			this.y = 0.0f;
			this.width = 0.0f;
			this.height = 0.0f;
		}

		internal ArrayList Attract()
		{
			int n;
			int ind1, ind2;
			ArrayList members = this.family.members;
			ArrayList branches = this.family.branches;
			ArrayList g = new ArrayList();
			g.Add(0.0f);
			g.Add(0.0f);

			if(this.vertices.Count == 0)		// Illegal program state.
				return null;

			if(branches.Count < 1) 
				return g;

			//
			// Connections with other branches. 
			//
			n = 0;
			for(int i = 0; i < bondings.Count; i++) 
			{
				ind1 = (int)bondings[i];
				ArrayList bonds = ((Member)members[ind1]).bonds;
				for(int k = 0; k < bonds.Count; k++) 
				{
					ind2 = (int)bonds[k];

					if(((Member)members[ind2]).tree == ((Member)members[ind1]).tree) 
						continue;

					if(((Member)members[ind2]).tree < 0) 
						continue;

					Branch b2 = (Branch)branches[((Member)members[ind2]).tree];

					Gforce(g, (x + ((Member)members[ind1]).x), (y + ((Member)members[ind1]).y),
						(b2.x + ((Member)members[ind2]).x), (b2.y + ((Member)members[ind2]).y));
					n++;
				}
			}

			//
			// Normalize gradient. 
			//
			g[0] = (float)g[0] / (1.0f + n);
			g[1] = (float)g[1] / (1.0f + n);
  
			//
			// Create random connection to ensure that branches stay together.
			//
			Random rand = new Random(Family.RANDOM_SEED);

			if(1.0 * rand.Next(RAND_MAX)/RAND_MAX * n < 1.0) 
			{
				ind1 = (rand.Next())%(branches.Count);
				Branch b = (Branch)branches[ind1];
				Rforce(g, (x + 0.5f * width), (y + 0.5f * height), (b.x + 0.5f * (b.width)), (b.y + 0.5f * (b.height)));
			}
  
			return g;
		}

		private void Gforce(ArrayList g, float x1, float y1, float x2, float y2) 
		{
			float dx = (x2 - x1);
			float dy = (y2 - y1);
			float r = (float)Math.Sqrt(dx*dx + dy*dy);
			dx /= (r + 1e-6f);
			dy /= (r + 1e-6f);
			if(r > 4.0)
				r = (4.0f + (float)Math.Sqrt(r - 4.0));
			g[0] = (float)g[0] + dx*r;
			g[1] = (float)g[1] + dy*r;
		}

		private void Rforce(ArrayList g, float x1, float y1, float x2, float y2) 
		{
			float dx = (x2 - x1);
			float dy = (y2 - y1);
			float r = (float)Math.Sqrt(dx*dx + dy*dy);
			dx /= (r + 1e-6f);
			dy /= (r + 1e-6f);
			if(r > 2.0) 
				r = (2.0f + (float)Math.Log10(r - 1.0));
			g[0] = (float)g[0] + dx*r;
			g[1] = (float)g[1] + dy*r;
		}

		internal void Connect()
		{
			int ind1, ind2;
			ArrayList members = this.family.members;

			//
			// Create bondings to other trees. 
			//
			this.bondings.Clear();

			for(int i = 0; i < this.vertices.Count; i++) 
			{
				ind1 = (int)this.vertices[i];
				ArrayList bonds = ((Member)members[ind1]).bonds;

				for(int k = 0; k < bonds.Count; k++) 
				{
					ind2 = (int)bonds[k]; 
					if(((Member)members[ind2]).tree == ((Member)members[ind1]).tree) 
						continue;
					this.bondings.Add(ind1);
					break;
				}
			}
		}

		internal ArrayList Repel(Branch branch)
		{
			float amp, r, dx, dy;
			ArrayList g = new ArrayList();
			g.Add(0.0);
			g.Add(0.0);

			if(this.family != branch.family) 
			{
				/*
				fprintf(stderr, "ERROR! %s at line %d: Illegal program state.\n",
					__FILE__, __LINE__);
				exit(1);
				*/
			}

			if(this == branch) 
				return g;

			//
			// Compute gradient. 
			//
			dx = (this.x - branch.x);
			dy = (this.y - branch.y);
			dx += 0.5f * (this.width - branch.width);
			dy += 0.5f * (this.height - branch.height);
			r = (dx*dx + dy*dy);
			amp = Overlap(this, branch);
			g[0] = amp*dx/r;
			g[1] = amp*dy/r;

			return g;
		}

		internal static float Overlap(Branch b1, Branch b2) 
		{
			float xmin1 = (b1.x - REPULSION_FIELD);
			float ymin1 = (b1.y - REPULSION_FIELD);
			float xmax1 = (b1.x + b1.width + REPULSION_FIELD);
			float ymax1 = (b1.y + b1.height + REPULSION_FIELD);
			float xmin2 = (b2.x - REPULSION_FIELD);
			float ymin2 = (b2.y - REPULSION_FIELD);
			float xmax2 = (b2.x + b2.width + REPULSION_FIELD);
			float ymax2 = (b2.y + b2.height + REPULSION_FIELD);
			float dx = 0.0f;
			float dy = 0.0f;

			if(xmin1 > xmax2) 
				dx = (xmin1 - xmax2); 
			if(xmin2 > xmax1) 
				dx = (xmin2 - xmax1); 
			if(ymin1 > ymax2) 
				dy = (ymin1 - ymax2); 
			if(ymin2 > ymax1) 
				dy = (ymin2 - ymax1); 

			return 1.0f/(dx*dx + dy*dy + 1e-10f);
		}

		internal void Update()
		{
			int j, k;
			float xp, yp; 
			float [] box = new float[4]{float.MaxValue, float.MaxValue, float.MinValue, float.MinValue};
			ArrayList members = this.family.members;

			//
			// Determine bounding box.
			//
			for(int i = 0; i < this.vertices.Count; i++) 
			{
				j = (int)vertices[i];
				xp = ((Member)members[j]).x;
				yp = ((Member)members[j]).y;

				if(xp < box[0]) 
					box[0] = xp;
				if(yp < box[1]) 
					box[1] = yp;

				xp += ((Member)members[j]).width;
				yp += ((Member)members[j]).height;

				if(((Member)members[j]).child >= 0) 
				{
					if((k = ((Member)members[((Member)members[j]).child]).father) == j)
						k = ((Member)members[((Member)members[j]).child]).mother;

					xp += ((Member)members[k]).width;
				}

				if(xp > box[2]) 
					box[2] = xp;
				if(yp > box[3]) 
					box[3] = yp;
			}

			//
			// Eliminate offsets.
			//
			for(int i = 0; i < this.vertices.Count; i++) 
			{
				j = (int)vertices[i];
				((Member)members[j]).x -= box[0];
				((Member)members[j]).y -= box[1];
			}

			this.width = (box[2] - box[0]);
			this.height = (box[3] - box[1]);
		}

		/// <summary>
		/// Convert a family member into a walker_vertex_t structure.
		/// </summary>
		/// <param name="fo">The current family.</param>
		/// <param name="i">The current member index.</param>
		/// <returns>The walker_vertex_t structure associated with the family member.</returns>
		internal static walker_vertex_t Member2vertex(Family fo, int i) 
		{
			int ind;
			walker_vertex_t v = new walker_vertex_t();
			ArrayList members = fo.members;

			if(i < 0) 
				return v;

			if(i >= members.Count) 
				return v;

			//
			// Default values. 
			//
			v.parent = ((Member)members[i]).parent;
			v.child = ((Member)members[i]).child;
			v.sibling = ((Member)members[i]).sibling;
			v.is_centering = (((Member)members[i]).is_original ? 1 : 0);
			v.x = ((Member)members[i]).x;
			v.y = ((Member)members[i]).y;
			v.width = ((Member)members[i]).width;
			v.height = ((Member)members[i]).height; 
			v.up_attach = ((Member)members[i]).up_attach;
			v.down_attach = ((Member)members[i]).width;
			v.user_data = null;

			//
			// Switch to vertex addresses. 
			//
			if((ind = v.parent) >= 0) 
				v.parent = ((Member)members[ind]).vertex;

			if((ind = v.child) >= 0) 
				v.child = ((Member)members[ind]).vertex;

			if((ind = v.sibling) >= 0) 
				v.sibling = ((Member)members[ind]).vertex;

			if(((Member)members[i]).tree < 0) 
				return v;

			if(((Member)members[i]).child < 0) 
				return v;

			//
			// Find coparent. 
			//
			if((ind = ((Member)members[((Member)members[i]).child]).father) == i)
				ind = ((Member)members[((Member)members[i]).child]).mother;

			v.width += ((Member)members[ind]).width;
  
			return v;
		}
		
		internal void Walk()
		{
			int k;
			int n = this.vertices.Count;

			walker_vertex_t [] wvtx = new walker_vertex_t[n + 1];
			ArrayList members = this.family.members;

			//
			// Create tree nodes. 
			//
			for(int i = 0; i < n; i++)
				wvtx[i] = Member2vertex(family, (int)vertices[i]);

			//
			// Make sure every node is connected at least to the root. 
			//
			wvtx[n] = new walker_vertex_t();
			wvtx[n].parent = -1;
			wvtx[n].child = -1;
			wvtx[n].sibling = -1;
			wvtx[n].is_centering = 1;
			wvtx[n].width = 1.0f;
			wvtx[n].height = 1.0f; 
			wvtx[n].up_attach = 0.5f;
			wvtx[n].down_attach = 0.5f;
			wvtx[n].user_data = null;
			for(int i = 0; i < n; i++) 
			{
				k = (n - i - 1);
				if(wvtx[k].parent >= 0) 
					continue;

				wvtx[k].parent = n;
				wvtx[k].sibling = wvtx[n].child;
				wvtx[n].child = k;
			}

			//
			// Start Walker II's node positioning algorithm. 
			//
			Walker(wvtx, (n + 1));

			for(int i = 0; i < n; i++) 
			{
				k = (int)vertices[i];
				((Member)members[k]).x = wvtx[i].x;
				((Member)members[k]).y = wvtx[i].y;
			}

			Update();
		}

		// *********************************************************************************************
		//
		//						WALKER II's NODE POSITIONING ALGORITHM
		//
		//			Walker II, John Q. "A Node-positioning Algorithm for General Trees"  
		//			Software Pract Exp, 1990, 20, 685-705
		// *********************************************************************************************

		/// <summary>
		/// Walker II's node positioning algorithm.
		/// </summary>
		/// <param name="vertices">The list of tree wertices specifying the tree topology.</param>
		/// <param name="n">The number of vertices.</param>
		/// <returns>True if everything is ok, false otherwise.</returns>
		private bool Walker(walker_vertex_t [] vertices, int n) 
		{
			int j;
			int first, last, left, left_sib;
			float height = 1.0f;
			node_t [] nodes;
			tree_t tree = new tree_t();
			int [] node2vtx;

			if(vertices == null) 
				return false;

			if(n < 1) 
				return false;

			if(n >= VERTEX_CAP) 
				n = VERTEX_CAP;

			tree.vtx2node = new int[n + 1];
			tree.node2vtx = new int[n + 1];
			tree.vertices = vertices;
			tree.nodes = new node_t[n + 1];
			tree.edge = new node_t[n + 1];

			for(int i = 0; i <= n; i++) 
			{
				tree.vtx2node[i] = -1;
				tree.node2vtx[i] = -1;
				tree.edge[i] = null;
			}

			//
			// Sort nodes according to their position in the tree and add links according to vertices.
			//
			tree.n = 0;
			tree.cap = n;
			tree.gen = 0;
			tree.depth = 0;

			for(int i = 0; i < n; i++) 
			{
				if(((walker_vertex_t)vertices[i]).height > height) 
					height = ((walker_vertex_t)vertices[i]).height;

				if(((walker_vertex_t)vertices[i]).parent >= 0) 
					continue;

				if(tree.depth > 0)		// Second root at i.
					return false;

				if (!WalkerDfs(tree, i))
					return false;

				WalkerBranch(tree, i);
			}

			//
			// Check that tree is connected. 
			//
			if(tree.n == 0)				// No root.
				return false;

			if(tree.n != n)				// Graph is not connected.
				return false;

			//
			// First part of the tree algorithm. 
			//
			nodes = tree.nodes;
            node2vtx = tree.node2vtx;

			for(int k = 0; k < tree.n; k++) 
			{
				first = ((node_t)nodes[k]).first_child;
				last = ((node_t)nodes[k]).last_child;
				left = ((node_t)nodes[k]).left_peer;

				//
				// Check left sibling. 
				//
				left_sib = -1;
				if(left >= 0)
					if(((node_t)nodes[left]).sibling == k)
						left_sib = left;

				//
				// Find the children to center above. 
				//
				while(first >= 0) 
				{
					j = node2vtx[first];
					if(((walker_vertex_t)vertices[j]).is_centering != 0) 
						break;

					if((first = ((node_t)nodes[first]).sibling) < 0)		// Parent at tree->node2vtx[k] cannot be centered.
						return false;
				}

				while(last >= 0) 
				{
					j = node2vtx[last];
					if(((walker_vertex_t)vertices[j]).is_centering != 0) 
						break;

					last = ((node_t)nodes[last]).left_peer;
				}

				//
				// Default position.
				//
				if(left_sib >= 0) 
				{
					((node_t)nodes[k]).x = ((node_t)nodes[left_sib]).x;
					((node_t)nodes[k]).x += ((node_t)nodes[left_sib]).width; 
				}

				//
				// Center above children and compute the distance to left sibling branches. 
				//
				if(first >= 0) 
				{
					j = node2vtx[k];
					if(left_sib < 0) 
					{
						((node_t)nodes[k]).x += (((node_t)nodes[first]).x + ((node_t)nodes[last]).x)/2.0f;
						((node_t)nodes[k]).x -= ((walker_vertex_t)vertices[j]).down_attach;
						first = node2vtx[first];
						last = node2vtx[last];
						((node_t)nodes[k]).x += (((walker_vertex_t)vertices[first]).up_attach)/2.0f;
						((node_t)nodes[k]).x += (((walker_vertex_t)vertices[last]).up_attach)/2.0f;
					}
					else 
					{
						((node_t)nodes[k]).xmod = (((node_t)nodes[k]).x + ((walker_vertex_t)vertices[j]).down_attach);
						((node_t)nodes[k]).xmod -= (((node_t)nodes[first]).x + ((node_t)nodes[last]).x)/2.0f;
						first = node2vtx[first];
						last = node2vtx[last];
						((node_t)nodes[k]).xmod -= (((walker_vertex_t)vertices[first]).up_attach)/2.0f;
						((node_t)nodes[k]).xmod -= (((walker_vertex_t)vertices[last]).up_attach)/2.0f;
					}
      
					//
					// Eliminate overlap with the left subtrees. 
					//
					first = WalkerTrace(tree, k, k);

					while((first < k) && (first >= 0)) 
					{
						WalkerMeasure(tree, first, k);
						first = WalkerTrace(tree, first, k);
					}
				}

				//
				// Balance subtrees.
				//
				if((left_sib >= 0) && (((node_t)nodes[k]).sibling < 0))
					WalkerBalance(tree, k);
			}
  
			//
			// Second part of the tree algorithm. 
			//
			tree.modifier = 0.0f;
			nodes = tree.nodes;
			for(int k = 0; k < tree.n; k++) 
			{
				if(((node_t)nodes[k]).parent >= 0) 
					continue;

				WalkerFix(tree, k);
			}

			//
			// Copy coordinates.
			//
			node2vtx = tree.node2vtx;
			nodes = tree.nodes;
			for(int k = 0; k < tree.n; k++) 
			{
				j = node2vtx[k];
				((walker_vertex_t)vertices[j]).x = ((node_t)nodes[k]).x;
				((walker_vertex_t)vertices[j]).y = height * (tree.depth - ((node_t)nodes[k]).gen);
			}

			return true;
		}

		/// <summary>
		/// Reset horizontal and compute vertical positions. The input index refers to the vertex array.
		/// </summary>
		/// <param name="tree">Current tree.</param>
		/// <param name="i">Current tree vertex index.</param>
		/// <returns>True if everything is ok, false otherwise.</returns>
		private bool WalkerDfs(tree_t tree, int i) 
		{
			int k = tree.n;
			int alpha = -1;
			int [] vtx2node = tree.vtx2node;
			int [] node2vtx = tree.node2vtx;
			walker_vertex_t [] vertices = tree.vertices;  
			node_t [] nodes = tree.nodes;

			//
			// Check vertex data. 
			//
			if(i < 0) 
				return true;

			if (!Check(tree, i))		// Check the current vertex.
				return false;

			if(tree.gen > tree.depth)
				tree.depth = tree.gen;

			//
			// Recursivly visit children. 
			//
			tree.gen += 1;
			if (!WalkerDfs(tree, vertices[i].child))
				return false;
			tree.gen -= 1;
			k = tree.n;
  
			//
			// Add new node. 
			//
			nodes[k] = new node_t();
			nodes[k].parent = -1;
			nodes[k].first_child = -1;
			nodes[k].last_child = -1;
			nodes[k].sibling = -1;
			nodes[k].left_peer = -1;
			nodes[k].right_peer = -1;
			nodes[k].contour = -1;
			nodes[k].gen = tree.gen;
			nodes[k].n_left_sibs = 0;
			nodes[k].x = 0.0f;
			nodes[k].xmod = 0.0f;
			nodes[k].xshift = 0.0f;
			nodes[k].xchange = 0.0f;
			nodes[k].width = vertices[i].width;
			vtx2node[i] = k;
			node2vtx[k] = i;
			tree.n += 1;

			//
			// Visit siblings if first-born. 
			//
			if((alpha = vertices[i].parent) < 0)
				return true;

			if(vertices[alpha].child != i)
				return true;

			while((i = vertices[i].sibling) >= 0)
				if (!WalkerDfs(tree, i))
					return false;

			return true;
		}

		/// <summary>
		/// Return the farthest sibling vertex. The input and output indices refer to the vertex array.
		/// </summary>
		/// <param name="tree">Current tree.</param>
		/// <param name="i">Current tree vertex index.</param>
		/// <returns>Return the farthest sibling vertex.</returns>
		private int WalkerBranch(tree_t tree, int i) 
		{
			int alpha, beta;
			int [] vtx2node = tree.vtx2node;
			int k = vtx2node[i];
			walker_vertex_t [] vertices = tree.vertices;  
			node_t [] nodes = tree.nodes;

			//
			// Set parent. 
			//
			if((alpha = ((walker_vertex_t)vertices[i]).parent) >= 0)
				nodes[k].parent = vtx2node[alpha];
  
			//
			// Update edge. 
			//
			alpha = k;
			beta = nodes[k].gen;
			if(tree.edge[beta] != null) 
			{
				for (int j = 0; j < nodes.Length; j++)
				{
					if (nodes[j].Equals(tree.edge[beta]))
					{
						alpha = j;
						break;
					}
				}

				nodes[k].left_peer = alpha;
				nodes[alpha].right_peer = k;
			}

			tree.edge[beta] = nodes[k];

			//
			// Visit children. 
			//
			beta = -1;
			if((alpha = ((walker_vertex_t)vertices[i]).child) >= 0)
				beta = WalkerBranch(tree, alpha);

			if(alpha >= 0) 
				nodes[k].first_child = vtx2node[alpha];
			if(beta >= 0) 
				nodes[k].last_child = vtx2node[beta];

			//
			// Visit siblings and count subtrees. 
			//
			beta = i;
			if((alpha = ((walker_vertex_t)vertices[i]).parent) < 0) 
				return beta;

			if(((walker_vertex_t)vertices[alpha]).child != i) 
				return beta;

			while((alpha = ((walker_vertex_t)vertices[beta]).sibling) >= 0) 
			{
				k = vtx2node[alpha];
				beta = vtx2node[beta];
				nodes[beta].sibling = k;
				nodes[k].n_left_sibs = (nodes[beta].n_left_sibs + 1);
				beta = WalkerBranch(tree, alpha);
			}

			return beta;
		}

		/// <summary>
		/// Find left contour thread. The input index refers to the node array.
		/// </summary>
		/// <param name="tree">Current tree.</param>
		/// <param name="k">Current tree vertex index.</param>
		/// <param name="apex"></param>
		/// <returns></returns>
		private int WalkerTrace(tree_t tree, int k, int apex) 
		{
			int alpha = -1;
			int beta = -1;
			int gen = -1;
			node_t [] nodes = tree.nodes;
			node_t [] edge = tree.edge;

			//
			// Check existing contour. 
			//
			if((alpha = nodes[k].contour) >= 0)
				return alpha;

			//
			// Check child. 
			//
			gen = nodes[k].gen + 1;
			if((alpha = nodes[k].first_child) >= 0) 
			{
				edge[gen] = nodes[alpha];
				nodes[k].contour = alpha;
				return alpha;
			}

			//
			// Check edge. 
			//
			if(edge[gen] == null)
				return -1;

			//
			// Trace left from the edge if the left edge peer belongs to the same subtree. 
			//
			for (int j = 0; j < nodes.Length; j++)
			{
				if (nodes[j].Equals(tree.edge[gen]))
				{
					alpha = j;
					break;
				}
			}

			beta = nodes[apex].left_peer;
			while((alpha > beta) && (alpha >= 0))
				alpha = nodes[alpha].left_peer;

			//
			// Trace right from the edge until current subtree. The contour can also point to the next subtree 
			// on the right if the current subtree is not deep enough.
			//
			while((alpha < beta) && (alpha >= 0)) 
			{
				edge[gen] = nodes[alpha];
				alpha = nodes[alpha].right_peer;
			}
			nodes[k].contour = alpha;

			return alpha;
		}

		/// <summary>
		/// Compute distance to left peer. If the closest mutual ancestor is above the current apex, 
		/// return FLT_MAX. The input index refers to the node array.
		/// </summary>
		/// <param name="tree"></param>
		/// <param name="k"></param>
		/// <param name="apex"></param>
		private void WalkerMeasure(tree_t tree, int k, int apex) 
		{
			int j;
			int peer = k;
			int alpha = -1;
			float dx = 0.0f;
			float lmod = 0.0f;
			float rmod = 0.0f;
			node_t [] nodes = tree.nodes;

			if((peer = nodes[k].left_peer) < 0) 
				return;

			//
			// Track left apex and update level modifier. 
			//
			alpha = -1;
			j = nodes[peer].parent;
			while((j >= 0) && (j < apex)) 
			{
				lmod += nodes[j].xmod;
				alpha = j;
				j = nodes[j].parent;
			}
  
			//
			// Check that left subtree is sibling. 
			//
			if(alpha < 0) 
				return;
			if(nodes[alpha].parent != nodes[apex].parent) 
				return;

			//
			// Track right modifier path. 
			//
			j = nodes[k].parent;
			while((j >= 0) && (j <= apex)) 
			{
				rmod += nodes[j].xmod;
				j = nodes[j].parent;
			}
  
			//
			// Compute distance. 
			//
			dx = (nodes[k].x - nodes[peer].x - nodes[peer].width);
			dx += (rmod - lmod);

			//
			// Adjust subtrees. 
			//
			if(dx < 0) 
			{
				k = (nodes[apex].n_left_sibs - nodes[alpha].n_left_sibs);
				nodes[apex].x -= dx;
				nodes[apex].xmod -= dx;
				nodes[apex].xshift -= dx;
				nodes[apex].xchange += dx/k;
				nodes[alpha].xchange -= dx/k; 
			}
		}

		/// <summary>
		/// Distribute middle subtrees evenly between the first and last subtrees. The input indices refer 
		/// to the node array.
		/// </summary>
		/// <param name="tree"></param>
		/// <param name="apex"></param>
		private void WalkerBalance(tree_t tree, int apex) 
		{
			int k = -1;
			int parent = -1;
			float xA, xB, cA, cB;
			node_t [] nodes = tree.nodes;

			xA = nodes[apex].xshift;
			cA = nodes[apex].xchange;	
			xB = 0.0f;
			cB = nodes[apex].xchange;
			parent = nodes[apex].parent;

			k = apex;
			while((k = nodes[k].left_peer) >= 0) 
			{
				if(nodes[k].parent != parent) 
					break;

				cA = nodes[k].xchange;
				xB = (xA + xB + cB);
				cB = (cA + cB);
				xA = nodes[k].xshift;
				nodes[k].x += xB;    
				nodes[k].xmod += xB;
			}
		}

		/// <summary>
		/// Apply modifiers to the preliminary positions. The input index refers to the node array.
		/// </summary>
		/// <param name="tree"></param>
		/// <param name="k"></param>
		private void WalkerFix(tree_t tree, int k) 
		{
			int alpha = -1;
			node_t [] nodes = tree.nodes;

			nodes[k].x += tree.modifier;
			if((alpha = nodes[k].first_child) >= 0) 
			{
				tree.modifier += nodes[k].xmod;
				WalkerFix(tree, alpha);
				tree.modifier -= nodes[k].xmod;
			}

			if((alpha = nodes[k].parent) < 0)
				return;

			if(nodes[alpha].first_child != k) 
				return;

			alpha = k;
			while((alpha = nodes[alpha].sibling) >= 0)
				WalkerFix(tree, alpha);
		}

		/// <summary>
		/// Check a vertex of a tree.
		/// </summary>
		/// <param name="tree">The tree to be checked.</param>
		/// <param name="i">The vertex to be checked index.</param>
		/// <returns>True if the checked vertex is ok, false otherwise.</returns>
		internal bool Check(tree_t tree, int i) 
		{
			int alpha = -1;
			int [] vtx2node = tree.vtx2node;
			walker_vertex_t [] vertices = tree.vertices;

			//
			// Check recursion. 
			//
			if(tree.gen >= MAX_DEPTH)	// Maximum recursion depth exceeded at i.
				return false;

			//
			// Check parent. 
			//
			if((alpha = ((walker_vertex_t)vertices[i]).parent) >= tree.cap)  // Invalid index at i->parent.
				return false;

			if(alpha == i)	// Parent to itself at i.
				return false;

			if(alpha >= 0)
			{
				if(vtx2node[alpha] >= 0)	// Cycle detected at i->parent.
					return false;
			}
  
			//
			// Check child. 
			//
			if((alpha = ((walker_vertex_t)vertices[i]).child) >= tree.cap)	// "Invalid index at i->child.
				return false;

			if(alpha == i)					// Child to itself at i.
				return false;

			if(alpha >= 0)
			{
				if(vtx2node[alpha] >= 0)	// Cycle detected at i->child.
					return false;
			}

			//
			// Check sibling. 
			//
			if((alpha = ((walker_vertex_t)vertices[i]).sibling) >= tree.cap)	// Invalid index at i->sibling.
				return false;

			if(alpha == i)					// Sibling to itself at i.
				return false;

			if(alpha >= 0)
			{
				if(vtx2node[alpha] >= 0)	// Cycle detected at i->sibling.
					return false;
			}

			//
			// Check width. 
			//
			if(((walker_vertex_t)vertices[i]).width < 0.0)						//  "Negative width at i.
				return false;

			if(((walker_vertex_t)vertices[i]).up_attach < 0.0)					// Negative up attachment at i.
				return false;

			if(((walker_vertex_t)vertices[i]).width < ((walker_vertex_t)vertices[i]).up_attach)		// Up attachment out of bounds at i.
				return false;

			if(((walker_vertex_t)vertices[i]).down_attach < 0.0)					// Negative down attachment at i.
				return false;

			if(((walker_vertex_t)vertices[i]).width < ((walker_vertex_t)vertices[i]).down_attach)	// Down attachment out of bounds at i.
				return false;

			return true;
		}
	}
}
