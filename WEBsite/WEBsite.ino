#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// Настройки Wi-Fi
const char* ssid = "MGTS_GPON_2220";        // Замените своим SSID
const char* password = "5PXM5TSX"; // Замените своим паролем Wi-Fi

// Настройки DHT


// Объявление сервера
WebServer server(80);

// Статические значения для датчиков MQ9 и MQ5
const float mq9Value = 200.0; // Замените на ваше значение
const float mq5Value = 150.0; // Замените на ваше значение

// Переменные для хранения значений DHT22 и светодатчика
float temperature;
float humidity;
int lightSensorValue;

// Функция для генерации случайных значений
void generateRandomValues() {
    temperature = random(20, 30);  // Случайная температура от 20 до 30 градусов
    humidity = random(40, 60);      // Случайная влажность от 40% до 60%
    lightSensorValue = random(0, 1023); // Случайное значение светочувствительного датчика
}
const char* patients[] = {"Иванов И. И.", "Петров П. П.", "Сидоров С. С."};
const int numPatients = sizeof(patients) / sizeof(patients[0]);
const char* htmlPage = R"rawliteral(
<!DOCTYPE HTML>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Умная палата</title>
    <style>
       body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            color: #333;
            text-align: center;
            margin: 0;
            padding: 0;
        }
        header {
            background-color: #4CAF50;
            color: white;
            padding: 10px 0;
        }
        .container {
            max-width: 600px;
            margin: 20px auto;
            padding: 20px;
            background-color: white;
            border-radius: 5px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        .status {
            font-size: 24px;
            margin: 10px 0;
        }
        .button {
            display: inline-block;
            padding: 10px 20px;
            margin: 10px;
            font-size: 16px;
            color: white;
            background-color: #4CAF50;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        .button:hover {
            background-color: #45a049;
        }
        select {
            font-size: 18px;
            padding: 5px;
            margin-top: 10px;
        }
        @media (max-width: 600px) {
            .status {
                font-size: 20px;
            }
            .button {
                font-size: 14px;
                padding: 8px 16px;
            }
        }
    </style>
</head>
<body>
    <header>
        <h1>Умная палата №1</h1>
    </header>
    <div class="container">
        <h2>Выберите пациента</h2>
         <select id="mySelect">
        <option value="1">Локтионов Д. В.</option>
        <option value="2">Корнеев В. Д.</option>
        <option value="3">Кравченко С. Б.</option>
        <option value="4">Гуровский И. Д.</option>
        <option value="5">Мартынов Д. С.</option>
    </select>
     <button class="button" id="confirmButton">Подтвердить выбор</button>
    <h2  id="selectedName">Состояние пациента.</h2>
    <script>
    // Функция для отображения выбранного элемента
function showSelectedOption() {
    // Находим элемент select по ID
    var selectElement = document.getElementById('mySelect');

    // Получаем выбранное значение
    var selectedValue = selectElement.options[selectElement.selectedIndex].text;

    // Отображаем выбранную фамилию
    document.getElementById('selectedName').innerText = "Состояние пациента " + selectedValue;
}

// Привязываем событие клика к кнопке
document.getElementById('confirmButton').addEventListener('click', showSelectedOption);
</script>
        
        <div class="status">Температура: <span id="temperature">36.6</span> °C</div>
        <div class="status">Пульс: <span id="pulse">72</span> уд/мин</div>
        <h2>Датчики палаты №1</h2>
        <div class="status">Температура в палате:<span id="dht_temp">24 </span> °C</div>
        <div class="status">Влажность в палате: <span id="dht_humidity">30</span> %</div>
        <div class="status">Alarms: None</div>
        <button class="button" onclick="alert('Кнопка была нажата, дежурный уже в пути!')">Кнопка вызова</button>
    </div>
</body>
</html>
)rawliteral";
// Обработчик главной страницы
void handleRoot() {
  String patientOptions = "";
    for (int i = 0; i < numPatients; i++) {
        patientOptions += "<option value=\"" + String(patients[i]) + "\">" + String(patients[i]) + "</option>";
    }
  String currentPage = htmlPage; 
  
     currentPage.replace("%PATIENT_OPTIONS%", patientOptions); // 
    String dhtTemp = String(temperature);
    String dhtHumidity = String(humidity);
    
    currentPage.replace("id=\"dht_temp\">--</span>", "id=\"dht_temp\">" + dhtTemp + "</span>");
    currentPage.replace("id=\"dht_humidity\">--</span>", "id=\"dht_humidity\">" + dhtHumidity + "</span>");
    server.send(200, "text/html", htmlPage); // Отправка HTML-страницы
}

void setup() {
    Serial.begin(115200);


    // Подключение к Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Подключение к WiFi...");
    }
    Serial.println("Подключено к WiFi");
    // Печатаем IP-адрес
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());
    // Начало веб-сервера
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP сервер запущен");

    // Генерация случайных значений в начале
    generateRandomValues();
}

void loop() {
    server.handleClient();

    // Каждую минуту генерируем новые случайные значения
    static unsigned long lastTime = millis();
    if (millis() - lastTime > 60000) {
        generateRandomValues();
        lastTime = millis();
    }
}
