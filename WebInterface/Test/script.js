document.addEventListener('DOMContentLoaded', function() {

    /**
     * ------------------
     * КРУГОВОЙ ТАЙМЕР
     * ------------------
     */
    const progressBar = document.querySelector('.progress-bar');
    const timerText = document.querySelector('.timer-text');

    // Находим радиус и вычисляем длину окружности
    const radius = progressBar.r.baseVal.value;
    const circumference = 2 * Math.PI * radius;
    progressBar.style.strokeDasharray = circumference;

    let totalTime = 5 * 60; // Общее время цикла в секундах (5 минут)
    let remainingTime = 3 * 60 + 33; // Текущее время (3:33)

    function updateTimer() {
        if (remainingTime < 0) {
            remainingTime = totalTime; // Перезапускаем таймер
        }

        // Обновляем текст
        const minutes = Math.floor(remainingTime / 60);
        const seconds = remainingTime % 60;
        timerText.textContent = `${minutes}:${seconds.toString().padStart(2, '0')}`;

        // Обновляем прогресс-бар
        const progress = (totalTime - remainingTime) / totalTime;
        const offset = circumference * (1 - progress);
        progressBar.style.strokeDashoffset = offset;

        remainingTime--;
    }

    // Запускаем таймер с обновлением каждую секунду
    setInterval(updateTimer, 1000);
    updateTimer(); // Первый вызов, чтобы не ждать секунду


    /**
     * ------------------
     * ГРАФИК (Chart.js)
     * ------------------
     */
    const ctx = document.getElementById('tempChart').getContext('2d');

    const tempChart = new Chart(ctx, {
        type: 'line', // Тип графика
        data: {
            labels: ['5m', '4m', '3m', '2m', '1m', 'Now'],
            datasets: [{
                label: 'Температура',
                data: [20, 26, 22, 27, 23, 26], // Пример данных
                borderColor: '#FFA500', // Оранжевый цвет линии
                backgroundColor: 'rgba(255, 165, 0, 0.1)', // Легкая заливка под линией
                fill: true,
                tension: 0.4, // Сглаживание линии
                borderWidth: 3,
                pointRadius: 0 // Убираем точки
            }, {
                label: 'Целевое значение',
                data: [24, 24, 24, 24, 24, 24], // Горизонтальная линия
                borderColor: 'rgba(255, 255, 255, 0.4)',
                borderWidth: 2,
                borderDash: [5, 5], // Пунктирная линия
                pointRadius: 0,
                fill: false,
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    display: false // Скрываем легенду (подписи данных)
                },
                tooltip: {
                    enabled: false // Отключаем всплывающие подсказки
                }
            },
            scales: {
                x: {
                    grid: {
                        display: false, // Скрываем сетку по оси X
                        drawBorder: false
                    },
                    ticks: {
                        color: '#999' // Цвет подписей на оси X
                    }
                },
                y: {
                    min: 15, // Минимальное значение на оси Y
                    max: 42, // Максимальное значение на оси Y
                    grid: {
                        display: false, // Скрываем сетку по оси Y
                        drawBorder: false
                    },
                    ticks: {
                        display: false // Скрываем подписи на оси Y
                    }
                }
            }
        }
    });
});