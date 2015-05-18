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
using System.Drawing;

namespace PedigreeDraw
{
	/// <summary>
	/// DLL interface class.
	/// </summary>
	public class Pedigree
	{
		/// <summary>
		/// The text font size.
		/// </summary>
		public const float FONT_SIZE = 10f;

		public const float BASE_FONT_WIDTH = 0.1f;

		/// <summary>
		/// The pedigree chart affection colors set - up to six affection supported.
		/// </summary>
		public static Brush [] FILL_COLORS = new Brush [] {Brushes.Red, 
															Brushes.Blue, 
															Brushes.Yellow, 
															Brushes.Lime, 
															Brushes.Cyan, 
															Brushes.Magenta};

		/// <summary>
		/// Stores the families list as pairs on names and Family object.
		/// </summary>
		private Hashtable families;

		/// <summary>
		/// Stores the emblems list as pairs of names and Emblem object.
		/// </summary>
		private Hashtable emblems;

		/// <summary>
		/// Stores the names of the fields containing additional text information.
		/// </summary>
		private ArrayList arTextFieldsNames;
		/// <summary>
		/// Stores the names of the fields containing affection represented by colors.
		/// </summary>
		private ArrayList arColorFieldsNames;

		/// <summary>
		/// Stores the current currentFamily to be drawn.
		/// </summary>
		private Family currentFamily;

		/// <summary>
		/// The canvas to draw onto.
		/// </summary>
		private Graphics g;

		/// <summary>
		/// The scaling factor used to convert the computed currentFamily pedigree layout in screen points.
		/// </summary>
		private float scale = 50f;
		/// <summary>
		/// The empty left margin used to display the legend.
		/// </summary>
		private float leftMargin = 100f;
		/// <summary>
		/// The symbol size in points.
		/// </summary>
		private float symbolSize = 25f;

		/// <summary>
		/// The pedigree display this.zoom.
		/// </summary>
		private float zoom = 1.0f;
		/// <summary>
		/// The font used to write the text.
		/// </summary>
		private Font font = new Font("arial", FONT_SIZE);
		/// <summary>
		/// Stores the canvas size.
		/// </summary>
		private Size canvasSize;

		/// <summary>
		/// Create an instance of Pedigree object class.
		/// </summary>
		/// <param name="persons">The list of persons to be drawn as pedigree charts. Instances of class Person.</param>
		public Pedigree(ArrayList persons)
		{
			//
			// Initialization.
			//
			this.arColorFieldsNames = new ArrayList();
			this.arTextFieldsNames = new ArrayList();
			this.arTextFieldsNames.Add("id");
			this.arTextFieldsNames.Add("age");

			this.emblems = new Hashtable();
			ArrayList vertices = new ArrayList();

			Vertex v;
			Emblem e;

			float font_width = 0.24f * FONT_SIZE * BASE_FONT_WIDTH;
			float width;

			foreach (Person p in persons)
			{
				// Create the vertex.
				v = new Vertex(p.Name, p.Family, p.Father, p.Mother);
				if (p.Age > 0)
					v.age = (float)p.Age;

				//
				// Update vertex dimensions. 
				//
				foreach(string line in p.Text) 
				{
					width = line.Length * font_width;
					if(width > 1.8) 
						width += 0.2f;

					if(width > v.width) 
						v.width = width;
				}

				v.height += 2f * font_width * (p.Text.Count + 0.5f);

				vertices.Add(v);

				// Create the emblem.
				e = new Emblem(p.Shape, p.Text, p.SlashFeature, p.ArrowFeature, p.Colors);
				e.Age = p.Age.ToString();
				this.emblems.Add(p.Name, e);

			}	// end foreach person

			//
			// Create the families.
			//
			this.families = Family.Create(vertices);
		}

		/// <summary>
		/// Application main entry point. Generates the pedigree trees.
		/// </summary>
		public void Run()
		{
			foreach (object key in this.families.Keys)
			{
				Family fam = (Family)this.families[key];
				fam.Simulate();
			}
		}

		/// <summary>
		/// Gets the number of individuals.
		/// </summary>
		public int Count { get { return this.emblems.Count; } }

		/// <summary>
		/// Gets the list of families.
		/// </summary>
		public Hashtable Families { get { return this.families; } }

		/// <summary>
		/// Get Family object by name.
		/// </summary>
		/// <param name="name">Family name.</param>
		/// <returns>The found currentFamily object, or null if not found.</returns>
		public Family GetFamily(string name)
		{
			if (this.families.ContainsKey(name))
				return (Family)this.families[name];

			return null;
		}

		/// <summary>
		/// Gets / sets the current currentFamily.
		/// </summary>
		public Family CurrentFamily
		{
			get { return this.currentFamily; }
			set 
			{ 
				this.currentFamily = value; 

				this.canvasSize = new Size(Convert.ToInt32(this.scale *this.zoom * this.currentFamily.Width + this.leftMargin), 
					Convert.ToInt32(this.scale * this.zoom * this.currentFamily.Height));
			}
		}

		/// <summary>
		/// Get Emblem object by name.
		/// </summary>
		/// <param name="name">Emblem name.</param>
		/// <returns>The found emblem object, or null if not found.</returns>
		public Emblem GetEmblem(string name)
		{
			if (this.emblems.ContainsKey(name))
				return (Emblem)this.emblems[name];

			return null;
		}

		/// <summary>
		/// Sets the text info fields names which will be displayed in the legend.
		/// </summary>
		public ArrayList  TextFieldNames	{ set { this.arTextFieldsNames = value; } }
		/// <summary>
		/// Sets the affection fields names which will be displayed in the legend as colors.
		/// </summary>
		public ArrayList AffectionFieldNames	{ set { this.arColorFieldsNames = value; } }

		/************************************************************************************************/
		/*																								*/
		/*																								*/
		/*								DRAW THE PEDIGREE ON THE SCREEN.								*/
		/*																								*/
		/*																								*/
		/************************************************************************************************/

		/// <summary>
		/// Sets the scale factor.
		/// </summary>
		public float Scale { set { this.scale = value; } }
		/// <summary>
		/// Sets the drawing left margin.
		/// </summary>
		public float LeftMargin { set { this.leftMargin = value; } }

		/// <summary>
		/// Sets the current zoom. A value of 100 represents no zoom, smaller positive values to zoom out
		/// and bigger values to zoom in.
		/// </summary>
		public int Zoom 
		{ 
			set 
			{ 
				this.zoom = value / 100.0f; 

				//
				// Reinitialization.
				//
				this.symbolSize = this.scale * this.zoom / 2.0f;
				float font_size = PedigreeDraw.Pedigree.FONT_SIZE * (float)(0.2 + value / 125.0);
				if (font_size < 6f)
					font_size = 6f;
				this.font = new Font("arial", font_size);

				this.canvasSize = new Size(Convert.ToInt32(this.scale * this.zoom * this.currentFamily.Width + this.leftMargin), 
					Convert.ToInt32(this.scale * this.zoom * this.currentFamily.Height));
			} 
		}

		/// <summary>
		/// Gets the current family canvas size.
		/// </summary>
		public Size CanvasSize { get { return this.canvasSize; } }

		/// <summary>
		/// Draw a family pedigree on the canvas.
		/// </summary>
		public void Print(Graphics canvas)
		{
			this.g = canvas;

			//
			// Write currentFamily id and number of individuals.
			//
			this.g.Clear(Color.White);
			this.g.DrawString("Family: " + this.currentFamily.Name, new Font("arial", 16), Brushes.Black, 10, 10);
			this.g.DrawString(this.currentFamily.Size + " individuals", new Font("arial", 12), Brushes.Black, 10, 35);

			DrawEdges();
			DrawLinks();
			DrawNodes();
			DrawLegend();
		}

		/// <summary>
		/// Draw the currentFamily pedigree lines.
		/// </summary>
		private void DrawEdges() 
		{
			ArrayList nodes = this.currentFamily.Nodes;

			int j;
			float x, y;
			  
			for(int i = 0; i < nodes.Count; i++) 
			{
				//
				// Connect parents. 
				//
				this.g.DrawLine(Pens.Black, this.leftMargin + this.scale * this.zoom * ((PedigreeDraw.Node)nodes[i]).x_a, 
					this.scale * this.zoom * (this.currentFamily.Height - ((PedigreeDraw.Node)nodes[i]).y),
					this.leftMargin + this.scale * this.zoom * ((PedigreeDraw.Node)nodes[i]).x_b, 
					this.scale * this.zoom * (this.currentFamily.Height - ((PedigreeDraw.Node)nodes[i]).y));

				//
				// Draw lines to children. 
				//
				ArrayList children = ((PedigreeDraw.Node)nodes[i]).children;
				for(int k = 0; k < children.Count; k++) 
				{
					j = (int)children[k];

					//
					// Middle siblings - vertical segement from each middle children upward to the siblings connector.
					//
					if(k > 0) 
					{
						if((k + 1) == children.Count) 
							break;

						x = this.leftMargin + this.scale * this.zoom * ((PedigreeDraw.Node)nodes[j]).x_a;
						y = this.scale * this.zoom * (this.currentFamily.Height - ((PedigreeDraw.Node)nodes[j]).y);

						this.g.DrawLine(Pens.Black, x, y, x, (y - this.scale * this.zoom * 1.0f));
						continue;
					}

					//
					// Central connector - vertical segement from the middle of the parents connector downward. 
					//
					x = this.leftMargin + this.scale * this.zoom * (((PedigreeDraw.Node)nodes[i]).x_b - 1.0f);
					y = this.scale * this.zoom * (this.currentFamily.Height - ((PedigreeDraw.Node)nodes[i]).y);

					this.g.DrawLine(Pens.Black, x, y, x, 
						this.scale * this.zoom * (this.currentFamily.Height - ((PedigreeDraw.Node)nodes[j]).y - 1.0f));

					//
					// First sibling - vertical segment from the first childern in each sibship upward to the 
					// siblings connector.
					//
					x = this.leftMargin + this.scale * this.zoom * ((PedigreeDraw.Node)nodes[j]).x_a;
					y = this.scale * this.zoom * (this.currentFamily.Height - ((PedigreeDraw.Node)nodes[j]).y);

					this.g.DrawLine(Pens.Black, x, y, x, (y - this.scale * this.zoom * 1.0f));
					
					j = (int)children[children.Count - 1];
					//
					// Last sibling - vertical segment from the last childern in each sibship upward to the 
					// siblings connector.
					//
					if(children.Count > 1) 
						this.g.DrawLine(Pens.Black, this.leftMargin + this.scale * this.zoom * ((PedigreeDraw.Node)nodes[j]).x_a, (y - this.scale * this.zoom * 1.0f), 
							this.leftMargin + this.scale * this.zoom * ((PedigreeDraw.Node)nodes[j]).x_a, y);

					//
					// Horizontal connector - connects all sibling together. 
					//
					this.g.DrawLine(Pens.Black, x, (y - this.scale * this.zoom * 1.0f), 
						this.leftMargin + this.scale * this.zoom * ((PedigreeDraw.Node)nodes[j]).x_a, (y - this.scale * this.zoom * 1.0f));
				}	// end for each child.
			}
		}

		/// <summary>
		/// Draw the currentFamily pedigree links and halos between duplicates individuals.
		/// </summary>
		private void DrawLinks() 
		{
			int i, k, j, ind;
			ArrayList nodes = this.currentFamily.Nodes;

			//
			// Randomize order to avoid biased overlaps. 
			//
			int n_trees = 0;
			int [] order = new int[nodes.Count];

			for(i = 0; i < nodes.Count; i++)
				order[i] = i;

			for(i = 0; i < nodes.Count; i++) 
			{
				Random rnd = new Random();
				k = rnd.Next()%(nodes.Count);

				ind = order[k];
				order[k] = order[i];
				order[i] = ind;

				if(n_trees <= ((PedigreeDraw.Node)nodes[i]).tree)
					n_trees = ((PedigreeDraw.Node)nodes[i]).tree + 1;
			}

			//
			// Draw links to other nodes. 
			//
			for(ind = 0; ind < nodes.Count; ind++) 
			{
				i = order[ind];
				if(((PedigreeDraw.Node)nodes[i]).origin_a == false) 
					continue;

				ArrayList links = ((PedigreeDraw.Node)nodes[i]).links;
				for(k = 0; k < links.Count; k++) 
				{
					j = (int)links[k];

					float hue = 1.0f * (((PedigreeDraw.Node)nodes[i]).tree)/n_trees;

					//
					// Extra currentFamily unit. 
					//
					if(((PedigreeDraw.Node)nodes[j]).alpha == ((PedigreeDraw.Node)nodes[i]).alpha) 
					{
						DrawArc(((PedigreeDraw.Node)nodes[j]).x_a, ((PedigreeDraw.Node)nodes[j]).y,
							((PedigreeDraw.Node)nodes[i]).x_a, ((PedigreeDraw.Node)nodes[i]).y, hue);
						continue;
					}

					// 
					// Ordinary duplicate. 
					//
					if(((PedigreeDraw.Node)nodes[j]).beta == ((PedigreeDraw.Node)nodes[i]).alpha) 
					{
						DrawArc(((PedigreeDraw.Node)nodes[j]).x_b, ((PedigreeDraw.Node)nodes[j]).y,
							((PedigreeDraw.Node)nodes[i]).x_a, ((PedigreeDraw.Node)nodes[i]).y, hue);
						continue;
					}

					DrawArc(((PedigreeDraw.Node)nodes[j]).x_b, ((PedigreeDraw.Node)nodes[j]).y,
						((PedigreeDraw.Node)nodes[i]).x_b, ((PedigreeDraw.Node)nodes[i]).y, hue);
				}
			}

			//
			// Draw background halos. 
			//
			for(ind = 0; ind < nodes.Count; ind++) 
			{
				i = order[ind];
				if(((PedigreeDraw.Node)nodes[i]).origin_a == false) 
					continue;

				ArrayList links = ((PedigreeDraw.Node)nodes[i]).links;

				for(k = 0; k < links.Count; k++) 
				{
					j = (int)links[k];

					//
					// Get node shape. 
					//
					Emblem emb;
					string key = ((PedigreeDraw.Node)nodes[i]).alpha;
					if((emb = this.GetEmblem(key)) == null)
						return;
				      
					//
					// Extra currentFamily unit. 
					//
					float hue = 1.0f * ((PedigreeDraw.Node)nodes[i]).tree/n_trees;

					if(((PedigreeDraw.Node)nodes[j]).alpha == ((PedigreeDraw.Node)nodes[i]).alpha) 
					{
						DrawHalo(((PedigreeDraw.Node)nodes[j]).x_a, ((PedigreeDraw.Node)nodes[j]).y, hue, emb);
						DrawHalo(((PedigreeDraw.Node)nodes[i]).x_a, ((PedigreeDraw.Node)nodes[i]).y, hue, emb);
						continue;
					}

					//
					// Ordinary duplicate. 
					//
					if(((PedigreeDraw.Node)nodes[j]).beta == ((PedigreeDraw.Node)nodes[i]).alpha) 
					{
						DrawHalo(((PedigreeDraw.Node)nodes[j]).x_b, ((PedigreeDraw.Node)nodes[j]).y, hue, emb);
						DrawHalo(((PedigreeDraw.Node)nodes[i]).x_a, ((PedigreeDraw.Node)nodes[i]).y, hue, emb);
						continue;
					}

					key = ((PedigreeDraw.Node)nodes[i]).beta;
					if((emb = this.GetEmblem(key)) == null)
						return;

					DrawHalo(((PedigreeDraw.Node)nodes[j]).x_b, ((PedigreeDraw.Node)nodes[j]).y, hue, emb);
					DrawHalo(((PedigreeDraw.Node)nodes[i]).x_b, ((PedigreeDraw.Node)nodes[i]).y, hue, emb);
				}
			}
		}


		/// <summary>
		/// Draw an eliptical colored arc between two points.
		/// </summary>
		/// <param name="x1">First point x coordinate.</param>
		/// <param name="y1">First point y coordinate.</param>
		/// <param name="x2">Second point x coordinate.</param>
		/// <param name="y2">Second point y coordinate.</param>
		/// <param name="color">Drawing color.</param>
		private void DrawArc(float x1, float y1, float x2, float y2, float color)
		{
			PointF p1, p2, c1, c2;
			
			p1 = new PointF(this.leftMargin + this.scale * this.zoom * x1, this.scale * this.zoom * (this.currentFamily.Height - y1));
			p2 = new PointF(this.leftMargin + this.scale * this.zoom * x2, this.scale * this.zoom * (this.currentFamily.Height - y2));

			if (p1.Y > p2.Y)
				c1 = new PointF(p1.X, p1.Y - 2 * this.scale * this.zoom);
			else
				c1 = new PointF(p1.X, p1.Y + 2 * this.scale * this.zoom);

			if (p1.X > p2.X)
				c2 = new PointF(p2.X + 2 * this.scale * this.zoom, p2.Y);
			else
				c2 = new PointF(p2.X - 2 * this.scale * this.zoom, p2.Y);

			this.g.DrawBezier(Pens.Red, p1, c1, c2, p2);
		}

		/// <summary>
		/// Draw a halo around the simbol representing an individual when is duplicated.
		/// </summary>
		/// <param name="x">The simbol representing an individual x coordinate.</param>
		/// <param name="y">The simbol representing an individual y coordinate.</param>
		/// <param name="ind">The halo color.</param>
		/// <param name="emblem">The symbol type.</param>
		private void DrawHalo(float x, float y, float ind, Emblem emblem) 
		{
			float x1, y1;
		
			//
			// Symbol upper left corner.
			//
			x1 = this.leftMargin + this.scale * this.zoom * x - this.symbolSize / 2f - 3;
			y1 = this.scale * this.zoom * (this.currentFamily.Height - y) - this.symbolSize / 2f - 3;

			//
			// Clear the symbol region.
			//
			this.g.FillRectangle(Brushes.White, x1, y1, this.symbolSize + 6f, this.symbolSize + 6f);

			switch(emblem.Shape)
			{
				case PedigreeDraw.Shapes.Square:
					this.g.DrawRectangle(Pens.Red, x1, y1, this.symbolSize + 6f, this.symbolSize + 6f);
					break;
				case PedigreeDraw.Shapes.Circle:
					this.g.DrawEllipse(Pens.Red, x1, y1, this.symbolSize + 6f, this.symbolSize + 6f);
					break;
				case PedigreeDraw.Shapes.Diamond:
					this.g.DrawPolygon(Pens.Red, new PointF [] { new PointF(this.scale * this.zoom * x, this.scale * this.zoom * (this.currentFamily.Height - y) - this.symbolSize / 2f - 3f),
																   new PointF(this.scale * this.zoom * x + this.symbolSize / 2f + 3f, this.scale * this.zoom * (this.currentFamily.Height - y)), 
																   new PointF(this.scale * this.zoom * x, this.scale * this.zoom * (this.currentFamily.Height - y) + this.symbolSize / 2f + 3f),
																   new PointF(this.scale * this.zoom * x - this.symbolSize / 2f - 3f, this.scale * this.zoom * (this.currentFamily.Height - y))});
					break;
			}
		}

		/// <summary>
		/// Draw the currentFamily pedigree symbols representing the individuals and the associated information.
		/// </summary>
		private void DrawNodes() 
		{
			ArrayList nodes = this.currentFamily.Nodes;
			Emblem emb;

			//
			// Draw nodes. 
			//
			for(int i = 0; i < nodes.Count; i++) 
			{
				string key_a = ((PedigreeDraw.Node)nodes[i]).alpha;
				string key_b = ((PedigreeDraw.Node)nodes[i]).beta;

				if(key_a != "" && (emb = this.GetEmblem(key_a)) != null) 
				{
					DrawEmblem(key_a, emb, ((PedigreeDraw.Node)nodes[i]).x_a, ((PedigreeDraw.Node)nodes[i]).y);
					DrawText(emb.Text, ((PedigreeDraw.Node)nodes[i]).x_a, ((PedigreeDraw.Node)nodes[i]).y);
				}

				if(key_b != ""  && (emb = this.GetEmblem(key_b)) != null) 
				{
					DrawEmblem(key_b, emb, ((PedigreeDraw.Node)nodes[i]).x_b, ((PedigreeDraw.Node)nodes[i]).y);
					DrawText(emb.Text, ((PedigreeDraw.Node)nodes[i]).x_b, ((PedigreeDraw.Node)nodes[i]).y);
				}
			}
		}

		/// <summary>
		/// Draw the symbol representing an individual in the currentFamily pedigree.
		/// </summary>
		/// <param name="id">Symbol id.</param>
		/// <param name="emblem">The symbol type.</param>
		/// <param name="x">The symbol x coordinate.</param>
		/// <param name="y">The symbol y coordinate.</param>
		private void DrawEmblem(string id, Emblem emblem, float x, float y) 
		{
			float x1, y1;
			Pen pen = Pens.Black;
			PointF [] points;
		
			//
			// Symbol upper left corner.
			//
			x1 = this.leftMargin + this.scale * this.zoom * x - this.symbolSize / 2f;
			y1 = this.scale * this.zoom * (this.currentFamily.Height - y) - this.symbolSize / 2f;

			//
			// Node outline and colors. 
			//
			switch(emblem.Shape)
			{
				case PedigreeDraw.Shapes.Square:
					this.g.FillRectangle(Brushes.White, x1, y1, this.symbolSize, this.symbolSize);
					if (emblem.Colors.Count > 0)
						ColorSquare(x1, y1, emblem.Colors);
					this.g.DrawRectangle(pen, x1, y1, this.symbolSize, this.symbolSize);
					break;
				case PedigreeDraw.Shapes.Circle:
					this.g.FillEllipse(Brushes.White, x1, y1, this.symbolSize, this.symbolSize);
					if (emblem.Colors.Count > 0)
						ColorCircle(x1, y1, emblem.Colors);
					this.g.DrawEllipse(pen, x1, y1, this.symbolSize, this.symbolSize);
					break;
				case PedigreeDraw.Shapes.Diamond:
					points = new PointF [] { new PointF(this.leftMargin + this.scale * this.zoom * x, this.scale * this.zoom * (this.currentFamily.Height - y) - this.symbolSize / 2f),
															  new PointF(this.leftMargin + this.scale * this.zoom * x + this.symbolSize / 2f, this.scale * this.zoom * (this.currentFamily.Height - y)), 
															  new PointF(this.leftMargin + this.scale * this.zoom * x, this.scale * this.zoom * (this.currentFamily.Height - y) + this.symbolSize / 2f),
															  new PointF(this.leftMargin + this.scale * this.zoom * x - this.symbolSize / 2f, this.scale * this.zoom * (this.currentFamily.Height - y))};
					this.g.FillPolygon(Brushes.White, points);
					this.g.DrawPolygon(pen, points);
					break;
			}

			//
			// Slash. 
			//
			if(emblem.SlashFeature) 
				this.g.DrawLine(pen, x1 + this.symbolSize * 1.2f, y1 - this.symbolSize * 0.2f, 
					x1 - this.symbolSize * 0.2f, y1 + this.symbolSize * 1.2f);

			//
			// Arrow. 
			//
			PointF p1, p2, p3;
			if(emblem.ArrowFeature) 
			{
				p1 = new PointF(x1 - this.symbolSize * 0.3f, y1 + this.symbolSize * 0.65f);
				p2 = new PointF(x1 + this.symbolSize * 0.2f, y1 + this.symbolSize * 0.90f);
				p3 = new PointF(x1 - this.symbolSize * 0.3f, y1 + this.symbolSize * 1.15f);

				this.g.FillPolygon(Brushes.Black, new PointF [] {p1, p2, p3});
			}
			
			//
			// Text start upper left corner.
			//
			x1 = this.leftMargin + this.scale * this.zoom * x - 1.5f * this.symbolSize / 2f;
			y1 = this.scale * this.zoom * (this.currentFamily.Height - y) + this.symbolSize / 2f + 0.5f * PedigreeDraw.Pedigree.FONT_SIZE;

			//
			// Write the individual's id.
			//
			this.g.DrawString(id, this.font, Brushes.Black, x1, y1);
			y1 += PedigreeDraw.Pedigree.FONT_SIZE * 1.2f;

			//
			// Write the age if exists.
			//
			if (emblem.Age != "-1")
			{
				this.g.DrawString(emblem.Age, this.font, Brushes.Black, x1, y1);
				y1 += PedigreeDraw.Pedigree.FONT_SIZE * 1.2f;
			}
		}

		/// <summary>
		/// Color the circle-shaped symbol.
		/// </summary>
		/// <param name="x">Symbol upper left x coordinate.</param>
		/// <param name="y">Symbol upper left y coordinate.</param>
		/// <param name="colors">The set of colors.</param>
		private void ColorCircle(float x, float y, ArrayList colors)
		{
			int angle = 360 / colors.Count;

			for (int i = 0; i < colors.Count; i++)
				this.g.FillPie((Brush)colors[i], x, y, this.symbolSize, this.symbolSize, (90 + i * angle)%360, angle);
		}

		/// <summary>
		/// Color the square-shaped symbol with up to 6 colors.
		/// </summary>
		/// <param name="x">Symbol upper left x coordinate.</param>
		/// <param name="y">Symbol upper left y coordinate.</param>
		/// <param name="colors">The set of colors.</param>
		private void ColorSquare(float x, float y, ArrayList colors)
		{
			switch(colors.Count)
			{
				case 0:			// No color fill.
					break;
				case 1:			// One color fill.
					this.g.FillRectangle((Brush)colors[0], x, y, this.symbolSize, this.symbolSize);
					break;
				case 2:			// Two colors fill.
					this.g.FillRectangle((Brush)colors[0], x, y, this.symbolSize / 2f, this.symbolSize);
					this.g.FillRectangle((Brush)colors[1], x + this.symbolSize / 2f, y, this.symbolSize / 2f, this.symbolSize);
					break;
				case 3:			// Three colors fill.
					this.g.FillRectangle((Brush)colors[0], x, y, this.symbolSize / 2f, this.symbolSize);
					this.g.FillRectangle((Brush)colors[1], x + this.symbolSize / 2f, y, this.symbolSize / 2f, this.symbolSize / 2f);
					this.g.FillRectangle((Brush)colors[2], x + this.symbolSize / 2f, y + this.symbolSize / 2f, this.symbolSize / 2f, this.symbolSize / 2f);
					break;
				case 4:			// Four colors fill.
					this.g.FillRectangle((Brush)colors[0], x, y, this.symbolSize / 2f, this.symbolSize / 2f);
					this.g.FillRectangle((Brush)colors[1], x, y + this.symbolSize / 2f, this.symbolSize / 2f, this.symbolSize / 2f);
					this.g.FillRectangle((Brush)colors[2], x + this.symbolSize / 2f, y, this.symbolSize / 2f, this.symbolSize / 2f);
					this.g.FillRectangle((Brush)colors[3], x + this.symbolSize / 2f, y + this.symbolSize / 2f, this.symbolSize / 2f, this.symbolSize / 2f);
					break;
				case 5:			// Five colors fill.
					this.g.FillRectangle((Brush)colors[0], x, y, this.symbolSize / 2f, this.symbolSize / 2f);
					this.g.FillRectangle((Brush)colors[1], x, y + this.symbolSize / 2f, this.symbolSize / 2f, this.symbolSize / 2f);
					this.g.FillRectangle((Brush)colors[2], x + this.symbolSize / 2f, y, this.symbolSize / 2f, this.symbolSize / 3f);
					this.g.FillRectangle((Brush)colors[3], x + this.symbolSize / 2f, y + this.symbolSize / 3f, this.symbolSize / 2f, this.symbolSize / 3f);
					this.g.FillRectangle((Brush)colors[4], x + this.symbolSize / 2f, y + 2f * this.symbolSize / 3f, this.symbolSize / 2f, this.symbolSize / 3f);
					break;
				case 6:			// Six colors fill.
					this.g.FillRectangle((Brush)colors[0], x, y, this.symbolSize / 2f, this.symbolSize / 3f);
					this.g.FillRectangle((Brush)colors[1], x, y + this.symbolSize / 3f, this.symbolSize / 2f, this.symbolSize / 3f);
					this.g.FillRectangle((Brush)colors[2], x, y + 2f * this.symbolSize / 3f, this.symbolSize / 2f, this.symbolSize / 3f);
					this.g.FillRectangle((Brush)colors[3], x + this.symbolSize / 2f, y, this.symbolSize / 2f, this.symbolSize / 3f);
					this.g.FillRectangle((Brush)colors[4], x + this.symbolSize / 2f, y + this.symbolSize / 3f, this.symbolSize / 2f, this.symbolSize / 3f);
					this.g.FillRectangle((Brush)colors[5], x + this.symbolSize / 2f, y + 2f * this.symbolSize / 3f, this.symbolSize / 2f, this.symbolSize / 3f);
					break;
			}
		}

		/// <summary>
		/// Draw the symbol associated text.
		/// </summary>
		/// <param name="text">The lines of text to be drawn.</param>
		/// <param name="x">The symbol x coordinate.</param>
		/// <param name="y">The symbol y coordinate.</param>
		private void DrawText(ArrayList text, float x, float y) 
		{
			float x1, y1;
		
			//
			// Text start upper left corner.
			//
			x1 = this.leftMargin + this.scale * this.zoom * x - 1.5f * this.symbolSize / 2f;
			y1 = this.scale * this.zoom * (this.currentFamily.Height - y) + this.symbolSize / 2f + 2.9f * PedigreeDraw.Pedigree.FONT_SIZE;

			for(int i = 0; i < text.Count; i++) 
			{
				this.g.DrawString(text[i].ToString(), this.font, Brushes.Black, x1, y1);
				y1 += PedigreeDraw.Pedigree.FONT_SIZE * 1.2f;
			}
		}

		/// <summary>
		/// Draw the legend in the bottom left part of the chart. The legend size doesn't change with the zoom.
		/// </summary>
		private void DrawLegend()
		{
			//
			// Compute legend position.
			//
			float x = 10f;
			float y = this.currentFamily.Height * 50.0f * this.zoom -		// The chart height.
				10f -												// The bottom chart margin.
				60f -														// The symbol shape legend height.
				this.arTextFieldsNames.Count * 12f -						// The text fields name.
				this.arColorFieldsNames.Count * 22f -						// The color codes signification.
				45f;														// The additional features slash and arrow height.

			//
			// Draw the legend.
			//

			// The symbols - height = 50.
			this.g.DrawPolygon(Pens.Black, new PointF [] { new PointF(x + 17.6f, y), new PointF(x + 35.2f, y + 17.6f), 
															 new PointF(x + 17.6f, y + 35.2f), new PointF(x, y + 17.6f)});
			this.g.FillRectangle(Brushes.White, x + 20f, y + 20f, 30f, 30f);
			this.g.DrawRectangle(Pens.Black, x + 20f, y + 20f, 30f, 30f);
			this.g.DrawString("male", new Font("arial", 7f), Brushes.Black, x + 23f, y + 38f);
			this.g.FillEllipse(Brushes.White, x + 25f, y, 35f, 35f);
			this.g.DrawEllipse(Pens.Black, x + 25f, y, 35f, 35f);
			this.g.DrawString("female", new Font("arial", 7f), Brushes.Black, x + 27f, y + 12f);

			// The text.
			y += 60f;
			for(int i = 0; i < this.arTextFieldsNames.Count; i++)
			{
				this.g.DrawString(this.arTextFieldsNames[i].ToString(), new Font("arial", 10f), Brushes.Black, x + 10f, y);
				y += 12f;
			}
			y += 10;

			// The affection color codes.
			for (int i = 0; i < this.arColorFieldsNames.Count; i++)
			{
				this.g.FillEllipse(FILL_COLORS[i], x, y, 20f, 20f); 
				this.g.DrawEllipse(Pens.Black, x, y, 20f, 20f); 
				this.g.DrawString(this.arColorFieldsNames[i].ToString(), new Font("arial", 10f), Brushes.Black, x + 25f, y + 3f);
				y += 22;
			}

			// The additional features slash and arrow.
			this.g.DrawEllipse(Pens.Black, x, y, 20f, 20f);
			this.g.DrawLine(Pens.Black, x, y, x + 20f, y + 20f);
			this.g.DrawString("dead", new Font("arial", 10f), Brushes.Black, x + 25f, y + 2f);
			y += 22;
			this.g.FillPolygon(Brushes.Black, new PointF [] {new PointF(x + 2f,y), new PointF(x + 18f, y + 8f), new PointF(x + 2f, y + 16f)});
			this.g.DrawString("SNPed", new Font("arial", 10f), Brushes.Black, x + 25f, y);
		}	
	}
}
