using Syncfusion.Licensing;

namespace SpasticityClientV2
{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            SyncfusionLicenseProvider.RegisterLicense("NzM1MjU0QDMyMzAyZTMzMmUzMGNzTncwMXBQcUs0d3dPM1lTT0oyc1ZaVkxpTzlsZUs5eTFNTWJYZnF4L1U9");
            SyncfusionLicenseProvider.RegisterLicense("NRAiBiAaIQQuGjN/V0Z+X09EaFtFVmJLYVB3WmpQdldgdVRMZVVbQX9PIiBoS35RdERjWXZfd3dRR2NeV0V1");
            // To customize application configuration such as set high DPI settings or default font,
            // see https://aka.ms/applicationconfiguration.
            ApplicationConfiguration.Initialize();
            Application.Run(new ChartModel());
        }
    }
}