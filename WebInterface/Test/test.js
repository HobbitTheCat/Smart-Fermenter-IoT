document.addEventListener('DOMContentLoaded', async function () {
    const progressBar = document.querySelector('.progress-bar');
    const timerText = document.querySelector('.timer-text');

    const radius = progressBar.r.baseVal.value;
    const circumference = 2 * Math.PI * radius;
    progressBar.style.strokeDasharray = circumference;

    let totalTime = 5 * 60; // Замените при необходимости динамически

    let chart; // Chart.js график

    async function fetchStatusAndUpdate() {
        try {
            const response = await fetch('/api/status');
            const status = await response.json();

            const timeLeft = status.timeLeft;
            const minutes = Math.floor(timeLeft / 60);
            const seconds = timeLeft % 60;
            timerText.textContent = `${minutes}:${seconds.toString().padStart(2, '0')}`;

            // обновление прогресс-бара
            const progress = (totalTime - timeLeft) / totalTime;
            const offset = circumference * (1 - progress);
            progressBar.style.strokeDashoffset = offset;
        } catch (e) {
            console.error('Ошибка при получении /api/status', e);
        }
    }

    async function fetchChartAndUpdate() {
        try {
            const response = await fetch('/api/temp_history');
            const temps = await response.json();
            const labels = temps.map((_, i) => `${temps.length - i}s`).reverse();

            if (chart) {
                chart.data.labels = labels;
                chart.data.datasets[0].data = temps;
                chart.update();
            } else {
                const ctx = document.getElementById('tempChart').getContext('2d');
                chart = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels,
                        datasets: [{
                            label: 'Температура',
                            data: temps,
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
                            tooltip: { enabled: false }
                        },
                        scales: {
                            x: {
                                grid: { display: false },
                                ticks: { color: '#999' }
                            },
                            y: {
                                min: 15,
                                max: 45,
                                grid: { display: false },
                                ticks: { display: false }
                            }
                        }
                    }
                });
            }
        } catch (e) {
            console.error('Ошибка при получении /api/temp_history', e);
        }
    }

    // Первый запуск
    await fetchStatusAndUpdate();
    await fetchChartAndUpdate();

    // Обновление каждую секунду и каждые 3 секунды графика
    setInterval(fetchStatusAndUpdate, 1000);
    setInterval(fetchChartAndUpdate, 3000);
});