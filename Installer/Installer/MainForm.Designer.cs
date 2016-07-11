namespace Installer
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.pathGroupBox = new System.Windows.Forms.GroupBox();
            this.selectButton = new System.Windows.Forms.Button();
            this.pathTextBox = new System.Windows.Forms.TextBox();
            this.optionsGroupBox = new System.Windows.Forms.GroupBox();
            this.shebangCheckBox = new System.Windows.Forms.CheckBox();
            this.associateFileTypesCheckBox = new System.Windows.Forms.CheckBox();
            this.addToPathCheckBox = new System.Windows.Forms.CheckBox();
            this.installCheckBox = new System.Windows.Forms.CheckBox();
            this.installButton = new System.Windows.Forms.Button();
            this.pathGroupBox.SuspendLayout();
            this.optionsGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // pathGroupBox
            // 
            this.pathGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pathGroupBox.Controls.Add(this.selectButton);
            this.pathGroupBox.Controls.Add(this.pathTextBox);
            this.pathGroupBox.Location = new System.Drawing.Point(13, 13);
            this.pathGroupBox.Name = "pathGroupBox";
            this.pathGroupBox.Size = new System.Drawing.Size(599, 51);
            this.pathGroupBox.TabIndex = 0;
            this.pathGroupBox.TabStop = false;
            this.pathGroupBox.Text = "Installation path";
            // 
            // selectButton
            // 
            this.selectButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.selectButton.Location = new System.Drawing.Point(518, 18);
            this.selectButton.Name = "selectButton";
            this.selectButton.Size = new System.Drawing.Size(75, 23);
            this.selectButton.TabIndex = 1;
            this.selectButton.Text = "Select";
            this.selectButton.UseVisualStyleBackColor = true;
            this.selectButton.Click += new System.EventHandler(this.selectButton_Click);
            // 
            // pathTextBox
            // 
            this.pathTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pathTextBox.Location = new System.Drawing.Point(7, 20);
            this.pathTextBox.Name = "pathTextBox";
            this.pathTextBox.Size = new System.Drawing.Size(505, 20);
            this.pathTextBox.TabIndex = 0;
            // 
            // optionsGroupBox
            // 
            this.optionsGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.optionsGroupBox.Controls.Add(this.shebangCheckBox);
            this.optionsGroupBox.Controls.Add(this.associateFileTypesCheckBox);
            this.optionsGroupBox.Controls.Add(this.addToPathCheckBox);
            this.optionsGroupBox.Controls.Add(this.installCheckBox);
            this.optionsGroupBox.Location = new System.Drawing.Point(13, 71);
            this.optionsGroupBox.Name = "optionsGroupBox";
            this.optionsGroupBox.Size = new System.Drawing.Size(599, 115);
            this.optionsGroupBox.TabIndex = 1;
            this.optionsGroupBox.TabStop = false;
            this.optionsGroupBox.Text = "Options";
            // 
            // shebangCheckBox
            // 
            this.shebangCheckBox.AutoSize = true;
            this.shebangCheckBox.Checked = true;
            this.shebangCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.shebangCheckBox.Location = new System.Drawing.Point(7, 92);
            this.shebangCheckBox.Name = "shebangCheckBox";
            this.shebangCheckBox.Size = new System.Drawing.Size(206, 17);
            this.shebangCheckBox.TabIndex = 3;
            this.shebangCheckBox.Text = "Provide shebang support for Windows";
            this.shebangCheckBox.UseVisualStyleBackColor = true;
            // 
            // associateFileTypesCheckBox
            // 
            this.associateFileTypesCheckBox.AutoSize = true;
            this.associateFileTypesCheckBox.Checked = true;
            this.associateFileTypesCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.associateFileTypesCheckBox.Location = new System.Drawing.Point(7, 68);
            this.associateFileTypesCheckBox.Name = "associateFileTypesCheckBox";
            this.associateFileTypesCheckBox.Size = new System.Drawing.Size(246, 17);
            this.associateFileTypesCheckBox.TabIndex = 2;
            this.associateFileTypesCheckBox.Text = "Associate known file types with Jaeger Toolset";
            this.associateFileTypesCheckBox.UseVisualStyleBackColor = true;
            // 
            // addToPathCheckBox
            // 
            this.addToPathCheckBox.AutoSize = true;
            this.addToPathCheckBox.Checked = true;
            this.addToPathCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.addToPathCheckBox.Location = new System.Drawing.Point(7, 44);
            this.addToPathCheckBox.Name = "addToPathCheckBox";
            this.addToPathCheckBox.Size = new System.Drawing.Size(190, 17);
            this.addToPathCheckBox.TabIndex = 1;
            this.addToPathCheckBox.Text = "Add to PATH environment variable";
            this.addToPathCheckBox.UseVisualStyleBackColor = true;
            // 
            // installCheckBox
            // 
            this.installCheckBox.AutoSize = true;
            this.installCheckBox.Checked = true;
            this.installCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.installCheckBox.Enabled = false;
            this.installCheckBox.Location = new System.Drawing.Point(7, 20);
            this.installCheckBox.Name = "installCheckBox";
            this.installCheckBox.Size = new System.Drawing.Size(74, 17);
            this.installCheckBox.TabIndex = 0;
            this.installCheckBox.Text = "Install files";
            this.installCheckBox.UseVisualStyleBackColor = true;
            // 
            // installButton
            // 
            this.installButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.installButton.Location = new System.Drawing.Point(13, 192);
            this.installButton.Name = "installButton";
            this.installButton.Size = new System.Drawing.Size(599, 23);
            this.installButton.TabIndex = 2;
            this.installButton.Text = "Install";
            this.installButton.UseVisualStyleBackColor = true;
            this.installButton.Click += new System.EventHandler(this.installButton_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(624, 227);
            this.Controls.Add(this.installButton);
            this.Controls.Add(this.optionsGroupBox);
            this.Controls.Add(this.pathGroupBox);
            this.MinimumSize = new System.Drawing.Size(640, 215);
            this.Name = "MainForm";
            this.Text = "Jaeger Toolset Installer";
            this.pathGroupBox.ResumeLayout(false);
            this.pathGroupBox.PerformLayout();
            this.optionsGroupBox.ResumeLayout(false);
            this.optionsGroupBox.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox pathGroupBox;
        private System.Windows.Forms.TextBox pathTextBox;
        private System.Windows.Forms.GroupBox optionsGroupBox;
        private System.Windows.Forms.CheckBox installCheckBox;
        private System.Windows.Forms.Button installButton;
        private System.Windows.Forms.Button selectButton;
        private System.Windows.Forms.CheckBox addToPathCheckBox;
        private System.Windows.Forms.CheckBox associateFileTypesCheckBox;
        private System.Windows.Forms.CheckBox shebangCheckBox;
    }
}

