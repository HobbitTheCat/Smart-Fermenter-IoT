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

    setInterval(fetchChartAndUpdate, 10000);
});
