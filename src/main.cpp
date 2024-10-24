#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "4G-MIFI-E4F";
const char* PASSWORD = "1234567890";


const char* SERVER_URL = "https://seu-app.herokuapp.com/dados";

#define D0 16
#define A0 35
#define NUM_READINGS 30


float leitura;
char payload[50] = "";

int readAnalogAverage(int pin) {
    long sum = 0;
    for (int i = 0; i < NUM_READINGS; i++) {
        sum += analogRead(pin);
        delay(10);
    }
    return sum / NUM_READINGS;
}

void initSerial() {
    Serial.begin(115200);
}

void initWiFi() {
    delay(10);
    Serial.println("------ Conexao WI-FI ------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.println(SSID);
    Serial.print("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void sendDataToFlask(float leitura) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(SERVER_URL); 
        http.addHeader("Content-Type", "application/json"); // Define o cabeçalho da requisição

        // Cria o payload em formato JSON
        String jsonPayload = String("{\"leitura\":") + leitura + "}";
        
        int httpResponseCode = http.POST(jsonPayload); // Envia a requisição POST

        // Verifica a resposta
        if (httpResponseCode > 0) {
            String response = http.getString(); // Obtém a resposta do servidor
            Serial.println(httpResponseCode); // Código de resposta HTTP
            Serial.println(response); // Resposta do servidor
        } else {
            Serial.print("Erro ao enviar: ");
            Serial.println(httpResponseCode);
        }
        
        http.end(); // Libera recursos
    } else {
        Serial.println("Erro: Wi-Fi não conectado");
    }
}

void setup() {
    initSerial();
    initWiFi();
    pinMode(D0, OUTPUT);
    digitalWrite(D0, HIGH);
}

void loop() {
    leitura = readAnalogAverage(A0);
    Serial.print("Leitura: ");
    Serial.println(leitura);
    
    sendDataToFlask(leitura);

    delay(1000);
}
