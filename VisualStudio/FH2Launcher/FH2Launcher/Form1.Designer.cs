namespace FH2Launcher
{
    partial class Form1
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
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.cbxFullscreen = new System.Windows.Forms.CheckBox();
            this.cbxSound = new System.Windows.Forms.CheckBox();
            this.numSound = new System.Windows.Forms.NumericUpDown();
            this.numVolume = new System.Windows.Forms.NumericUpDown();
            this.cbxMusic = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.comboResolutions = new System.Windows.Forms.ComboBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.button2 = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tbxConfig = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.downloadButton = new System.Windows.Forms.Button();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSound)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numVolume)).BeginInit();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Location = new System.Drawing.Point(13, 13);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(340, 329);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.cbxFullscreen);
            this.tabPage1.Controls.Add(this.cbxSound);
            this.tabPage1.Controls.Add(this.numSound);
            this.tabPage1.Controls.Add(this.numVolume);
            this.tabPage1.Controls.Add(this.cbxMusic);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Controls.Add(this.comboResolutions);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(332, 303);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "System";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // cbxFullscreen
            // 
            this.cbxFullscreen.AutoSize = true;
            this.cbxFullscreen.Location = new System.Drawing.Point(10, 60);
            this.cbxFullscreen.Name = "cbxFullscreen";
            this.cbxFullscreen.Size = new System.Drawing.Size(79, 17);
            this.cbxFullscreen.TabIndex = 6;
            this.cbxFullscreen.Text = "Full Screen";
            this.cbxFullscreen.UseVisualStyleBackColor = true;
            // 
            // cbxSound
            // 
            this.cbxSound.AutoSize = true;
            this.cbxSound.Location = new System.Drawing.Point(10, 128);
            this.cbxSound.Name = "cbxSound";
            this.cbxSound.Size = new System.Drawing.Size(134, 17);
            this.cbxSound.TabIndex = 5;
            this.cbxSound.Text = "Sound Volume: (0 - 10)";
            this.cbxSound.UseVisualStyleBackColor = true;
            // 
            // numSound
            // 
            this.numSound.Location = new System.Drawing.Point(206, 128);
            this.numSound.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numSound.Name = "numSound";
            this.numSound.Size = new System.Drawing.Size(120, 20);
            this.numSound.TabIndex = 4;
            this.numSound.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // numVolume
            // 
            this.numVolume.Location = new System.Drawing.Point(206, 91);
            this.numVolume.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numVolume.Name = "numVolume";
            this.numVolume.Size = new System.Drawing.Size(116, 20);
            this.numVolume.TabIndex = 3;
            this.numVolume.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // cbxMusic
            // 
            this.cbxMusic.AutoSize = true;
            this.cbxMusic.Location = new System.Drawing.Point(10, 92);
            this.cbxMusic.Name = "cbxMusic";
            this.cbxMusic.Size = new System.Drawing.Size(131, 17);
            this.cbxMusic.TabIndex = 2;
            this.cbxMusic.Text = "Music Volume: (0 - 10)";
            this.cbxMusic.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 7);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(60, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Resolution:";
            // 
            // comboResolutions
            // 
            this.comboResolutions.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboResolutions.FormattingEnabled = true;
            this.comboResolutions.Location = new System.Drawing.Point(10, 32);
            this.comboResolutions.Name = "comboResolutions";
            this.comboResolutions.Size = new System.Drawing.Size(316, 21);
            this.comboResolutions.TabIndex = 0;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.listBox1);
            this.tabPage2.Controls.Add(this.downloadButton);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(332, 303);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Downloads";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.button2);
            this.tabPage3.Controls.Add(this.groupBox1);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(332, 303);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Config";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(244, 268);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 1;
            this.button2.Text = "Save Config";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tbxConfig);
            this.groupBox1.Location = new System.Drawing.Point(6, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(320, 255);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Output Config";
            // 
            // tbxConfig
            // 
            this.tbxConfig.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxConfig.Location = new System.Drawing.Point(3, 16);
            this.tbxConfig.Multiline = true;
            this.tbxConfig.Name = "tbxConfig";
            this.tbxConfig.Size = new System.Drawing.Size(314, 236);
            this.tbxConfig.TabIndex = 0;
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.button1.Location = new System.Drawing.Point(261, 345);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(88, 34);
            this.button1.TabIndex = 1;
            this.button1.Text = "&Launch";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // downloadButton
            // 
            this.downloadButton.Location = new System.Drawing.Point(6, 6);
            this.downloadButton.Name = "downloadButton";
            this.downloadButton.Size = new System.Drawing.Size(123, 28);
            this.downloadButton.TabIndex = 1;
            this.downloadButton.Text = "Download maps";
            this.downloadButton.UseVisualStyleBackColor = true;
            this.downloadButton.Click += new System.EventHandler(this.button4_Click);
            // 
            // listBox1
            // 
            this.listBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Location = new System.Drawing.Point(7, 41);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(319, 251);
            this.listBox1.TabIndex = 2;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(365, 391);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.tabControl1);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Form1";
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSound)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numVolume)).EndInit();
            this.tabPage2.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox comboResolutions;
        private System.Windows.Forms.CheckBox cbxSound;
        private System.Windows.Forms.NumericUpDown numSound;
        private System.Windows.Forms.NumericUpDown numVolume;
        private System.Windows.Forms.CheckBox cbxMusic;
        private System.Windows.Forms.TextBox tbxConfig;
        private System.Windows.Forms.CheckBox cbxFullscreen;
        private System.Windows.Forms.Button downloadButton;
        private System.Windows.Forms.ListBox listBox1;
    }
}

