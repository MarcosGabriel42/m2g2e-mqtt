using MQTTnet.Client;
using MQTTnet;
using System.Text.Json;
using MQTTAplication.Data;

public class MqttService
{
    private readonly IMqttClient _mqttClient;

    public MqttService()
    {
        var factory = new MqttFactory();
        _mqttClient = factory.CreateMqttClient();
    }

    public async Task ConnectToBrokerAsync()
    {
        var options = new MqttClientOptionsBuilder()
            .WithClientId("Senac210")
            .WithTcpServer("zfeemqtt.eastus.cloudapp.azure.com", 41883) // Coloque a URL e a porta do broker
            .WithCredentials("Senac", "Senac")
            .WithCleanSession()
            .Build();

        await _mqttClient.ConnectAsync(options);
    }

    public async Task<(float temperature, float humidity)> GetWeatherDataAsync()
    {
        float temperature = 0;
        float humidity = 0;

        _mqttClient.ApplicationMessageReceivedAsync += e =>
        {
            var payload = System.Text.Encoding.UTF8.GetString(e.ApplicationMessage.Payload);
            var data = JsonSerializer.Deserialize<MqttData>(payload);

            temperature = data.Temperature;
            humidity = data.Humidity;

            return Task.CompletedTask;
        };

        // Subscreva ao tópico do broker
        await _mqttClient.SubscribeAsync("Senac/M2G2E/Saida");

        return (temperature, humidity);
        
    }
}
