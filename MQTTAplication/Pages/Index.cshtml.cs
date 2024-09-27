using Microsoft.AspNetCore.Mvc.RazorPages;
using MQTTAplication.Data;

namespace MQTTAplication.Pages
{
    public class IndexModel : PageModel
    {
        private readonly MqttService _mqttService;

        public List<MqttData> MqttData { get; set; }

        public IndexModel(MqttService mqttService)
        {
            _mqttService = mqttService;
            MqttData = new List<MqttData>();
        }

        public async Task OnGetAsync()
        {
            await _mqttService.ConnectToBrokerAsync();
            var (temperatura, umidade) = await _mqttService.GetWeatherDataAsync();

            MqttData.Add(new MqttData
            {
                Temperature = temperatura,
                Humidity = umidade,
                Timestamp = DateTime.Now
            });
            Console.WriteLine(MqttData.Count());
        }
    }
}