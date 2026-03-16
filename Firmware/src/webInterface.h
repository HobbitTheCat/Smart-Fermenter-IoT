#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <map>
#include "FSM/fsm.h"
#include "FSM/states/h/runningState.h"
#include "FSM/states/h/idleState.h"

#ifndef WEBINTERFACE
#define WEBINTERFACE

const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>IoT Dashboard</title>
    <style>
        :root {
            /* Размер плитки адаптируется под размер экрана */
            --tile-size: min(100vh/3, 100vw/4);
            --text-size: calc(var(--tile-size)*0.23);
            --gap: 15px;
            --main-bg: #2B2D30;
            --tile-bg: #43454A;
            --text-color: #EAEAEA;
            --accent-color: #FFA500; /* Оранжевый */
        }

        body {
            margin: 0;
            background-color: #1E1F22;
            color: var(--text-color);
            font-family: 'Segoe UI', Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }

        .tile-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .large-tile {

            background-color: var(--main-bg);
            padding: var(--gap);
            border-radius: 12px;
            box-shadow: 0 0 20px rgba(0, 0, 0, 0.6);
        }

        .tile-grid {
            display: grid;
            grid-template-columns: repeat(3, var(--tile-size));
            grid-template-rows: var(--tile-size) auto;
            gap: var(--gap);
        }

        .tile {
            border-radius: 8px;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
            padding: 15px;
            box-sizing: border-box;
        }

        .small-tile {
            background-color: var(--tile-bg);
            width: var(--tile-size);
            height: var(--tile-size);
        }

        .wide-tile {
            background-color: var(--tile-bg);
            grid-column: span 3; /* Растягиваем на 3 колонки */
            height: calc(var(--tile-size) * 0.8); /* Делаем график чуть ниже */
        }

        /* --- Стили для контента внутри плиток --- */

        .tile-content {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            height: 100%;
            text-align: center;
        }

        .tile-label {
            font-size: 1.1rem;
            color: #b0b0b0;
            margin-bottom: 10px;
        }

        /* Плитка 1: Target */
        .target-temp {
            font-size: var(--text-size);
            font-weight: bold;
        }
        .arrows {
            display: flex;
            gap: 20px;
            margin-top: 10px;
        }
        .arrow {
            width: 0;
            height: 0;
            border-left: 10px solid transparent;
            border-right: 10px solid transparent;
            cursor: pointer;
            opacity: 0.7;
            transition: opacity 0.2s;
        }
        .arrow:hover { opacity: 1; }
        .arrow.up { border-bottom: 15px solid var(--text-color); }
        .arrow.down { border-top: 15px solid var(--text-color); }


        /* Плитка 2: Таймер */
        .timer-container { position: relative; }
        .timer-svg {
            position: absolute;
            width: 90%;
            height: 90%;
            transform: rotate(-90deg) scaleX(-1); !* Начинаем сверху *!
            transform-origin: center;
        }
        .progress-bg, .progress-bar {
            fill: none;
            stroke-width: 10;
        }
        .progress-bg {
            stroke: var(--main-bg);
        }
        .progress-bar {
            stroke: var(--accent-color);
            stroke-linecap: round; /* Скругляем концы линии */
        }
        .timer-text {
            font-size: var(--text-size);
            font-weight: bold;
        }
        .timer-subtext {
            font-size: 1.2rem;
            color: #b0b0b0;
        }

        /* Плитка 3: Время */
        .time-value {
            font-size: var(--text-size);
            font-weight: bold;
        }
        .time-buttons {
            display: flex;
            gap: 10px;
            margin-top: 10px;
        }
        .time-buttons button {
            background: #5a5d63;
            color: var(--text-color);
            border: none;
            padding: 6px 12px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 0.9rem;
            transition: background 0.2s;
        }
        .time-buttons button:hover { background: #6b6e75; }

        /* Плитка 4: График */
        #tempChart {
            width: 100%;
            height: 100%;
        }
    </style>
</head>
<body>
<main>
    <div class="tile-container">
        <div class="tile large-tile">
            <div class="tile-grid">
                <div class="tile small-tile" id="internal1">
                    <div class="tile-content">
                        <span class="tile-label">Target</span>
                        <div class="target-temp">34°C</div>
                        <div class="arrows">
                            <div class="arrow down" onclick="setTargetTemperature(-1)"></div>
                            <div class="arrow up" onclick="setTargetTemperature(1)"></div>
                        </div>
                    </div>
                </div>
                <div class="tile small-tile" id="internal2" onclick="timerButtonHandler()">
                    <div class="tile-content timer-container">
                        <svg class="timer-svg" viewBox="0 0 120 120">
                            <circle class="progress-bg" cx="60" cy="60" r="54" />
                            <circle class="progress-bar" cx="60" cy="60" r="54" />
                        </svg>
                        <div class="timer-text">00:00</div>
                    </div>
                </div>
                <div class="tile small-tile" id="internal3">
                    <div class="tile-content">
                        <span class="tile-label">Time</span>
                        <div class="time-value">00:00</div>
                        <div class="time-buttons">
                            <button onclick="setTime(3600)">+1h</button>
                            <button onclick="setTime(300)">+5m</button>
                        </div>
                    </div>
                </div>
                <div class="tile wide-tile" id="internal4">
                    <canvas id="tempChart"></canvas>
                </div>
            </div>
        </div>
    </div>
</main>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script>
    const progressBar = document.querySelector('.progress-bar');
    const timerText = document.querySelector('.timer-text');
    const targetTime = document.querySelector('.time-value');
    const temperatureText = document.querySelector('.target-temp');

    const radius = progressBar.r.baseVal.value;
    const circumference = 2 * Math.PI * radius;
    progressBar.style.strokeDasharray = circumference;

    let interval = 600;
    let isRunning = false;
    let chart;


    let targetTemp = 10; // целевая температура, можно менять

    const dashedLinePlugin = {
        id: 'dashedLinePlugin',
        afterDatasetsDraw(chart, args, options) {
            const {ctx, chartArea, scales} = chart;
            const y = scales.y.getPixelForValue(targetTemp);
            ctx.save();
            ctx.beginPath();
            ctx.setLineDash([6, 6]); // длина штриха и пробела
            ctx.strokeStyle = 'red';
            ctx.lineWidth = 1;
            ctx.moveTo(chartArea.left, y);
            ctx.lineTo(chartArea.right, y);
            ctx.stroke();
            ctx.restore();
        }
    };


    async function getStatus(){
        const response = await fetch('/api/status');
        return await response.json();
    }

    async function pauseProcess(){
        const response = await fetch('/api/pause', {method: 'POST'});
        return await response.text();
    }

    async function startProcess(){
        const response = await fetch('/api/start', {method: 'POST'});
        return await response.text();
    }

    async function setTime(secondsToAdd){
        const formData = new URLSearchParams();
        formData.append("amount_to_add_sec", secondsToAdd);
        const response = await fetch('/api/set_time', {
            method: "POST",
            headers: {"Content-Type": "application/x-www-form-urlencoded"},
            body: formData
        });
        return await response.text();
    }

    async function setTargetTemperature(deltaTemp){
        const formData = new URLSearchParams();
        formData.append("temp_to_add", deltaTemp);
        const response = await fetch('/api/set_target', {
            method: "POST",
            headers: {"Content-Type": "application/x-www-form-urlencoded"},
            body: formData
        });
        return await response.text();
    }

    async function updateData(){
        try {
            const status = await getStatus();
            isRunning = status.isRunning;
            targetTemp = status.targetTemp;

            const timeLeft = status.timeLeft;
            const initialDuration = status.initialDuraion;

            let minutes = Math.floor(timeLeft / 60);
            let seconds = Math.floor(timeLeft % 60);
            let hours = Math.floor(minutes / 60);
            minutes = minutes % 60;

            if (hours > 0) timerText.textContent = `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}`;
            else timerText.textContent = `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;

            minutes = Math.floor(initialDuration / 60);
            hours = Math.floor(minutes / 60);
            minutes = minutes % 60;
            targetTime.textContent = `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}`;


            const progress = timeLeft / initialDuration;
            progressBar.style.strokeDashoffset = circumference * (1 - progress);

            if(targetTemp <= 10) temperatureText.textContent = `Off`
            else temperatureText.textContent = `${targetTemp.toFixed(2)}°C`;

        } catch (e) {
            console.error("Error while retrieving data", e);
        }
    }

    async function fetchChartAndUpdate() {
        try {
            const response = await fetch(`/api/temp_history?interval=${interval}`);
            const temps = await response.json();

            const labels = [];
            let lastLabelKey = null;

            temps.forEach(entry => {
                const t = entry.dt;

                let label;
                let labelKey;

                if (t < 60) {
                    label = `${t}s`;
                    labelKey = `s${t}`; // каждую секунду можно подписывать
                } else if (t < 3600) {
                    const m = Math.floor(t / 60);
                    label = `${m}m`;
                    labelKey = `m${m}`; // группа по минуте
                } else {
                    const h = Math.floor(t / 3600);
                    label = `${h}h`;
                    labelKey = `h${h}`; // группа по часу
                }

                if (labelKey !== lastLabelKey) {
                    labels.push(label);
                    lastLabelKey = labelKey;
                } else {
                    labels.push(""); // повтор — оставляем пустым
                }
            });


            const data = temps.map(entry => entry.temp);

            if (chart) {
                chart.data.labels = labels;
                chart.data.datasets[0].data = data;
                chart.update();
            } else {
                const ctx = document.getElementById('tempChart').getContext('2d');
                chart = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels,
                        datasets: [{
                            label: 'Температура',
                            data: data,
                            borderColor: '#FFA500',
                            backgroundColor: 'rgba(255,165,0,0.1)',
                            fill: true,
                            tension: 0.4,
                            borderWidth: 3,
                            pointRadius: 0
                        }]
                    },
                    options: {
                        responsive: true,
                        maintainAspectRatio: false,
                        plugins: {
                            legend: { display: false },
                            tooltip: { enabled: false },
                            dashedLinePlugin
                        },
                        scales: {
                            x: {
                                grid: { display: false },
                                ticks: {
                                    color: '#999',
                                    maxTicksLimit: 10
                                }
                            },
                            y: {
                                min: 10,
                                max: 42,
                                ticks: {
                                    stepSize: 5,
                                    color: '#666',
                                    display: true
                                },
                                grid: {
                                    display: true,
                                    color: '#eee'
                                }
                            }
                        }
                    },
                    plugins: [dashedLinePlugin]
                });
            }
        } catch (e) {
            console.error('Ошибка при получении /api/temp_history', e);
        }
    }

    async function updateInterval(newDuration) {
        interval = newDuration;
    }

    async function timerButtonHandler() {
        if (isRunning) {
            await pauseProcess();
        } else {
            await startProcess();
        }
    }

    window.addEventListener('DOMContentLoaded', () => {
        setInterval(updateData, 500);

        // setInterval(fetchChartAndUpdate, 5000);
    });

</script>
</body>
</html>
)rawliteral";



class WebInterface {
    private:
        const char* ssid;
        const char* password;
        FSM& fsm;

        AsyncWebServer server;

        void setupRoutes() {
            this->server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
                request->send_P(200, "text/html", MAIN_page);
            });

            server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
                request->send(204);
            });



            this->server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request){
                StaticJsonDocument<200> doc;
                doc["currentTemp"] = this->fsm.temperature.getTemperature();
                doc["targetTemp"] = this->fsm.tcu.getTargetTemperature();
                doc["initialDuraion"] = this->fsm.timer.getInitialDuration();
                doc["timeLeft"] = this->fsm.timer.getRemainingTime();
                doc["isRunning"] = this->fsm.isRunning();
                String json;
                serializeJson(doc, json);
                request->send(200, "application/json", json);
            });

            this->server.on("/api/temp_history", HTTP_GET, [this](AsyncWebServerRequest *request){
                if (!request->hasParam("interval")) {
                    request->send(400, "text/plain", "Missing 'interval' parameter");
                    return;
                }

                String intervalSizeStr = request->getParam("interval")->value();
                uint32_t interval = intervalSizeStr.toInt();
                if (interval == 0 || interval > 86400) {
                    request->send(400, "text/plain", "Invalid 'interval' value");
                    return;
                }

                uint32_t now = millis() / 1000;
                char json[2048];  // буфер JSON-ответа
                size_t len = snprintf(json, sizeof(json), "[");
                bool first = true;

                for (size_t i = 0; i < this->fsm.temperature.getHistoryCount(); ++i) {
                    const TempEntry& entry = this->fsm.temperature.getHistoryEntry(i);
                    if (entry.timestamp == 0 || entry.timestamp > now) continue;

                    uint32_t dt = now - entry.timestamp;
                    if (dt <= interval) {
                        if (!first) {
                            len += snprintf(json + len, sizeof(json) - len, ",");
                        }
                        len += snprintf(json + len, sizeof(json) - len,
                                        "{\"dt\":%lu,\"temp\":%.2f}", dt, entry.temperature);
                        first = false;
                    }
                }

                snprintf(json + len, sizeof(json) - len, "]");
                request->send(200, "application/json", json);
            });



            this->server.on("/api/start", HTTP_POST, [this](AsyncWebServerRequest *request){
               this->fsm.timer.resume();
               this->fsm.changeState(new RunningState());
               request->send(200, "text/plain", "Started"); 
            });

            this->server.on("/api/pause", HTTP_POST, [this](AsyncWebServerRequest *request){
                this->fsm.timer.pause();
                this->fsm.changeState(new IdleState());
                request->send(200, "text/plain", "Paused");
            });

            this->server.on("/api/set_time", HTTP_POST, [this](AsyncWebServerRequest *request){
                if (request->hasParam("amount_to_add_sec", true)) {
                    int remainingTime = this->fsm.timer.getRemainingTime()*1000;
                    remainingTime += request->getParam("amount_to_add_sec", true)->value().toInt() * 1000;
                    if (remainingTime < 0) remainingTime = 360000000 + remainingTime;
                    if (remainingTime >= 360000000) remainingTime -= 360000000;
                    this->fsm.timer.setRemainningTime(std::abs(remainingTime));

                    request->send(200, "text/plain", "Time updated");
                } else {
                    request->send(400, "text/plain",  "Missing parameter");
                }
            });

            this->server.on("/api/set_target", HTTP_POST, [this](AsyncWebServerRequest *request) {
                if (request->hasParam("temp_to_add", true)) {
                    float targetTemperature = this->fsm.tcu.getTargetTemperature();
                    targetTemperature += request->getParam("temp_to_add", true) -> value().toFloat();
                    if (targetTemperature < 9.5) targetTemperature = 45;
                    if (targetTemperature > 45) targetTemperature = 10;
                    this->fsm.tcu.setTargetTemperature(targetTemperature);
                    
                    request->send(200, "text/plain", "Target temperature updated");
                } else {
                    request->send(400, "text/plain", "Missing parameter");
                }
            });
        }

    public:
        WebInterface(FSM& fsm, const char* ssid, const char* password): password(password), ssid(ssid), server(80), fsm(fsm) {}

        void begin () {
            WiFi.begin(this->ssid, this->password);
            Serial.print("Connecting to Wifi");
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print("."); // возможно добавить вывод на экран
            }
            Serial.println("\nConnected to WiFi: " + WiFi.localIP().toString());

            this->setupRoutes();
            this->server.begin();
        }
};

#endif