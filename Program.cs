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
            SyncfusionLicenseProvider.RegisterLicense("NzM1MjMxQDMyMzAyZTMzMmUzMGNzTncwMXBQcUs0d3dPM1lTT0oyc1ZaVkxpTzlsZUs5eTFNTWJYZnF4L1U9;NzM1MjMyQDMyMzAyZTMzMmUzME4wUWhjS3JuL1hkUTNSR3lOaytmZlB0bWtTaGdzdVVHUU9nWjJ0VVJoeHc9;NRAiBiAaIQQuGjN/V0Z+X09EaFtKVmJLYVB3WmpQdldgdVRMZVVbQX9PIiBoS35RdERjWXZfdndRR2dUU0J2;NzM1MjM0QDMyMzAyZTMzMmUzMFNVU0I0S3FKVU91R0h2dnhFeitVdFoyWnJ6SGJJMkJkelpVY0RVbVlzZ0k9");
            // To customize application configuration such as set high DPI settings or default font,
            // see https://aka.ms/applicationconfiguration.
            ApplicationConfiguration.Initialize();
            Application.Run(new ChartModel());
        }
    }
}