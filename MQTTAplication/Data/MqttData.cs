
namespace MQTTAplication.Data
{
    public class MqttData
    {
        public float Temperature { get; set; }
        public float Humidity { get; set; }
        public DateTime Timestamp { get; internal set; }
    }
}
