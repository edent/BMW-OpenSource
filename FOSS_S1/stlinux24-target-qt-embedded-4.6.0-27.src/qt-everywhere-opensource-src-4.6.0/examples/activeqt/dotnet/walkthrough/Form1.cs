using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace csharp
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private AxwrapperaxLib.AxQPushButton resetButton;
		private AxmultipleaxLib.AxQAxWidget2 circleWidget;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Form1));
			this.resetButton = new AxwrapperaxLib.AxQPushButton();
			this.circleWidget = new AxmultipleaxLib.AxQAxWidget2();
			((System.ComponentModel.ISupportInitialize)(this.resetButton)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.circleWidget)).BeginInit();
			this.SuspendLayout();
			// 
			// resetButton
			// 
			this.resetButton.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right);
			this.resetButton.Enabled = true;
			this.resetButton.Location = new System.Drawing.Point(160, 296);
			this.resetButton.Name = "resetButton";
			this.resetButton.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("resetButton.OcxState")));
			this.resetButton.Size = new System.Drawing.Size(168, 32);
			this.resetButton.TabIndex = 1;
			this.resetButton.clicked += new System.EventHandler(this.resetLineWidth);
			// 
			// circleWidget
			// 
			this.circleWidget.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.circleWidget.Enabled = true;
			this.circleWidget.Location = new System.Drawing.Point(8, 8);
			this.circleWidget.Name = "circleWidget";
			this.circleWidget.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("circleWidget.OcxState")));
			this.circleWidget.Size = new System.Drawing.Size(320, 264);
			this.circleWidget.TabIndex = 2;
			this.circleWidget.ClickEvent += new System.EventHandler(this.circleClicked);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(336, 333);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.circleWidget,
																		  this.resetButton});
			this.Name = "Form1";
			this.Text = "Form1";
			((System.ComponentModel.ISupportInitialize)(this.resetButton)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.circleWidget)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

//! [0]
		private void circleClicked(object sender, System.EventArgs e)
		{
			this.circleWidget.lineWidth++;
		}
//! [0]

//! [1]
		private void resetLineWidth(object sender, System.EventArgs e)
		{
			this.circleWidget.lineWidth = 1;
			this.resetButton.setFocus();
		}	
//! [1]
	}
}
