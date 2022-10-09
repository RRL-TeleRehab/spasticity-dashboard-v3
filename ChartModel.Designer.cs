namespace SpasticityClientV2
{
    partial class ChartModel
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ChartModel));
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.serialData = new System.Windows.Forms.RichTextBox();
            this.comboBox2 = new System.Windows.Forms.ComboBox();
            this.GetPorts = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.nChartControl1 = new Nevron.Chart.WinForm.NChartControl();
            this.nChartControl2 = new Nevron.Chart.WinForm.NChartControl();
            this.nChartControl3 = new Nevron.Chart.WinForm.NChartControl();
            this.button3 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.SuspendLayout();
           
            // 
            // comboBox1
            // 
            this.comboBox1.Location = new System.Drawing.Point(0, 0);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(121, 23);
            this.comboBox1.TabIndex = 0;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(0, 0);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 0;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(0, 0);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 0;
            // 
            // serialData
            // 
            this.serialData.Location = new System.Drawing.Point(0, 0);
            this.serialData.Name = "serialData";
            this.serialData.Size = new System.Drawing.Size(100, 96);
            this.serialData.TabIndex = 0;
            this.serialData.Text = "";
            // 
            // comboBox2
            // 
            this.comboBox2.FormattingEnabled = true;
            this.comboBox2.Location = new System.Drawing.Point(513, 29);
            this.comboBox2.Name = "comboBox2";
            this.comboBox2.Size = new System.Drawing.Size(102, 23);
            this.comboBox2.TabIndex = 3;
            // 
            // GetPorts
            // 
            this.GetPorts.Location = new System.Drawing.Point(513, 70);
            this.GetPorts.Name = "GetPorts";
            this.GetPorts.Size = new System.Drawing.Size(102, 23);
            this.GetPorts.TabIndex = 4;
            this.GetPorts.Text = "Get COM Ports";
            this.GetPorts.UseVisualStyleBackColor = true;
            this.GetPorts.Click += new System.EventHandler(this.button3_Click);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(513, 100);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(102, 23);
            this.button4.TabIndex = 5;
            this.button4.Text = "Read";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // timer1
            // 
            this.timer1.Interval = 20;
            // 
            // nChartControl1
            // 
            this.nChartControl1.AutoRefresh = false;
            this.nChartControl1.BackColor = System.Drawing.SystemColors.Control;
            this.nChartControl1.InputKeys = new System.Windows.Forms.Keys[0];
            this.nChartControl1.Location = new System.Drawing.Point(12, 13);
            this.nChartControl1.Name = "nChartControl1";
            this.nChartControl1.Size = new System.Drawing.Size(474, 110);
            this.nChartControl1.State = ((Nevron.Chart.WinForm.NState)(resources.GetObject("nChartControl1.State")));
            this.nChartControl1.TabIndex = 6;
            this.nChartControl1.Text = "nChartControl1";
            // 
            // nChartControl2
            // 
            this.nChartControl2.AutoRefresh = false;
            this.nChartControl2.BackColor = System.Drawing.SystemColors.Control;
            this.nChartControl2.InputKeys = new System.Windows.Forms.Keys[0];
            this.nChartControl2.Location = new System.Drawing.Point(12, 129);
            this.nChartControl2.Name = "nChartControl2";
            this.nChartControl2.Size = new System.Drawing.Size(474, 110);
            this.nChartControl2.State = ((Nevron.Chart.WinForm.NState)(resources.GetObject("nChartControl2.State")));
            this.nChartControl2.TabIndex = 7;
            this.nChartControl2.Text = "nChartControl2";
            // 
            // nChartControl3
            // 
            this.nChartControl3.AutoRefresh = false;
            this.nChartControl3.BackColor = System.Drawing.SystemColors.Control;
            this.nChartControl3.InputKeys = new System.Windows.Forms.Keys[0];
            this.nChartControl3.Location = new System.Drawing.Point(12, 245);
            this.nChartControl3.Name = "nChartControl3";
            this.nChartControl3.Size = new System.Drawing.Size(474, 110);
            this.nChartControl3.State = ((Nevron.Chart.WinForm.NState)(resources.GetObject("nChartControl3.State")));
            this.nChartControl3.TabIndex = 8;
            this.nChartControl3.Text = "nChartControl3";
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(513, 158);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(102, 23);
            this.button3.TabIndex = 9;
            this.button3.Text = "Save Data";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click_1);
            // 
            // button5
            // 
            this.button5.Location = new System.Drawing.Point(513, 129);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(102, 23);
            this.button5.TabIndex = 10;
            this.button5.Text = "Stop";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // ChartModel
            // 
            this.ClientSize = new System.Drawing.Size(685, 372);
            this.Controls.Add(this.button5);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.nChartControl3);
            this.Controls.Add(this.nChartControl2);
            this.Controls.Add(this.nChartControl1);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.GetPorts);
            this.Controls.Add(this.comboBox2);
            this.Name = "ChartModel";
            this.ResumeLayout(false);

        }

        #endregion

        private ComboBox comboBox1;
        private Button button1;
        private Button button2;
        private RichTextBox serialData;
        private ComboBox comboBox2;
        private Button GetPorts;
        private Button button4;
        private System.Windows.Forms.Timer timer1;
        private Nevron.Chart.WinForm.NChartControl nChartControl1;
        private Nevron.Chart.WinForm.NChartControl nChartControl2;
        private Nevron.Chart.WinForm.NChartControl nChartControl3;
        private Button button3;
        private Button button5;
    }
}