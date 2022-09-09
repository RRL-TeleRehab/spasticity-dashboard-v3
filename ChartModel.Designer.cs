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
            this.cartesianChart1 = new LiveChartsCore.SkiaSharpView.WinForms.CartesianChart();
            this.measureModelBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.serialData = new System.Windows.Forms.RichTextBox();
            this.measureModelBindingSource1 = new System.Windows.Forms.BindingSource(this.components);
            this.measureModelBindingSource2 = new System.Windows.Forms.BindingSource(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.measureModelBindingSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.measureModelBindingSource1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.measureModelBindingSource2)).BeginInit();
            this.SuspendLayout();
            // 
            // cartesianChart1
            // 
            this.cartesianChart1.DataBindings.Add(new System.Windows.Forms.Binding("Tag", this.measureModelBindingSource, "Value", true));
            this.cartesianChart1.Font = new System.Drawing.Font("Cambria", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.cartesianChart1.Location = new System.Drawing.Point(54, 35);
            this.cartesianChart1.Name = "cartesianChart1";
            this.cartesianChart1.Size = new System.Drawing.Size(346, 262);
            this.cartesianChart1.TabIndex = 0;
            this.cartesianChart1.Load += new System.EventHandler(this.cartesianChart1_Load);
            // 
            // measureModelBindingSource
            // 
            this.measureModelBindingSource.DataSource = typeof(SpasticityClientV2.MeasureModel);
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Location = new System.Drawing.Point(836, 51);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(121, 23);
            this.comboBox1.TabIndex = 4;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(836, 80);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(121, 23);
            this.button1.TabIndex = 5;
            this.button1.Text = "Get COM Ports";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(836, 109);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 6;
            this.button2.Text = "Read";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // serialData
            // 
            this.serialData.Location = new System.Drawing.Point(54, 362);
            this.serialData.Name = "serialData";
            this.serialData.Size = new System.Drawing.Size(847, 96);
            this.serialData.TabIndex = 7;
            this.serialData.Text = "";
            this.serialData.TextChanged += new System.EventHandler(this.serialData_TextChanged);
            // 
            // measureModelBindingSource1
            // 
            this.measureModelBindingSource1.DataSource = typeof(SpasticityClientV2.MeasureModel);
            // 
            // measureModelBindingSource2
            // 
            this.measureModelBindingSource2.DataSource = typeof(SpasticityClientV2.MeasureModel);
            // 
            // ChartModel
            // 
            this.ClientSize = new System.Drawing.Size(1024, 574);
            this.Controls.Add(this.serialData);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.comboBox1);
            this.Controls.Add(this.cartesianChart1);
            this.Name = "ChartModel";
            this.Load += new System.EventHandler(this.ChartModel_Load);
            ((System.ComponentModel.ISupportInitialize)(this.measureModelBindingSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.measureModelBindingSource1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.measureModelBindingSource2)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private LiveChartsCore.SkiaSharpView.WinForms.CartesianChart cartesianChart1;
        private ComboBox comboBox1;
        private Button button1;
        private Button button2;
        private RichTextBox serialData;
        private BindingSource measureModelBindingSource;
        private BindingSource measureModelBindingSource1;
        private BindingSource measureModelBindingSource2;
    }
}