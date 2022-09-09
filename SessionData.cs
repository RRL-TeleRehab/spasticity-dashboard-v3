using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SpasticityClientV2
{
    public class SessionData
    {
        public long TimeStamp { get; set; }
        public float Angle_deg { get; set; }
        public float AngVel_degpersec { get; set; }
        public float EMG_mV { get; set; }
        public float Force_N { get; set; }
    }
}
