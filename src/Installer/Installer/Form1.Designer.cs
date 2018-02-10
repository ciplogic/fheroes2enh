namespace Installer
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
            this.label1 = new System.Windows.Forms.Label();
            this.installButton = new System.Windows.Forms.Button();
            this.licensesInstallCheck = new System.Windows.Forms.CheckBox();
            this.installOptionsUserControl1 = new Installer.UserControls.InstallOptionsUserControl();
            this.installerControlProgress1 = new Installer.UserControls.InstallerControlProgress();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Calibri", 14F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(13, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(201, 23);
            this.label1.TabIndex = 0;
            this.label1.Text = "Free Heroes 2 Enhanced";
            // 
            // installButton
            // 
            this.installButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.installButton.Location = new System.Drawing.Point(475, 243);
            this.installButton.Name = "installButton";
            this.installButton.Size = new System.Drawing.Size(75, 23);
            this.installButton.TabIndex = 1;
            this.installButton.Text = "Install";
            this.installButton.UseVisualStyleBackColor = true;
            this.installButton.Click += new System.EventHandler(this.button1_Click);
            // 
            // licensesInstallCheck
            // 
            this.licensesInstallCheck.AutoSize = true;
            this.licensesInstallCheck.Location = new System.Drawing.Point(17, 249);
            this.licensesInstallCheck.Name = "licensesInstallCheck";
            this.licensesInstallCheck.Size = new System.Drawing.Size(117, 17);
            this.licensesInstallCheck.TabIndex = 3;
            this.licensesInstallCheck.Text = "Agree with licenses";
            this.licensesInstallCheck.UseVisualStyleBackColor = true;
            this.licensesInstallCheck.CheckedChanged += new System.EventHandler(this.licensesInstallCheck_CheckedChanged);
            // 
            // installOptionsUserControl1
            // 
            this.installOptionsUserControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.installOptionsUserControl1.Location = new System.Drawing.Point(17, 40);
            this.installOptionsUserControl1.Name = "installOptionsUserControl1";
            this.installOptionsUserControl1.Size = new System.Drawing.Size(533, 197);
            this.installOptionsUserControl1.TabIndex = 2;
            // 
            // installerControlProgress1
            // 
            this.installerControlProgress1.Location = new System.Drawing.Point(17, 40);
            this.installerControlProgress1.Name = "installerControlProgress1";
            this.installerControlProgress1.Size = new System.Drawing.Size(533, 197);
            this.installerControlProgress1.TabIndex = 4;
            this.installerControlProgress1.Visible = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(562, 278);
            this.Controls.Add(this.installerControlProgress1);
            this.Controls.Add(this.licensesInstallCheck);
            this.Controls.Add(this.installOptionsUserControl1);
            this.Controls.Add(this.installButton);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Free Heroes 2 Enhanced Installer";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button installButton;
        private UserControls.InstallOptionsUserControl installOptionsUserControl1;
        private System.Windows.Forms.CheckBox licensesInstallCheck;
        private UserControls.InstallerControlProgress installerControlProgress1;
    }
}