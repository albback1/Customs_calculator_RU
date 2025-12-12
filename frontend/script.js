/* Тема сайта*/
document.addEventListener('DOMContentLoaded', function() {
    const themeToggle = document.getElementById('themeToggle');
    const savedTheme = localStorage.getItem('theme');
    
    // Применяем сохранённую тему
    if (savedTheme) {
        document.documentElement.setAttribute('data-theme', savedTheme);
    }
    
    // Обработчик клика
    themeToggle.addEventListener('click', function() {
        const currentTheme = document.documentElement.getAttribute('data-theme');
        const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
        
        document.documentElement.setAttribute('data-theme', newTheme);
        localStorage.setItem('theme', newTheme);
        
        // Меняем иконку
        themeToggle.textContent = newTheme === 'dark' ? '🌞 Светлая тема' : '🌙 Тёмная тема';
    });
    
    // Устанавливаем начальную иконку
    if (savedTheme === 'dark') {
        themeToggle.textContent = '🌞 Светлая тема';
    }
});



/* Видимость полей в зависимости от выбора типа двигателя / клиента */
document.addEventListener('DOMContentLoaded', function () {
    const engineTypeSelect = document.getElementById('engineType');
    const clientTypeSelect = document.getElementById('clientType');

    const engineCapacityField = document.getElementById('engineCapacity');
    const engineCapacityLabel = document.querySelector('label[for="engineCapacity"]');

    
    const horsePowerField = document.getElementById('horsePower');
    const horsePowerLabel = document.querySelector('label[for="horsePower"]');

    // Функция для обновления видимости полей
    function updateEngineCapacityVisibility() {
        if (engineTypeSelect.value === '1') { // электро
            engineCapacityField.style.display = 'none';
            engineCapacityLabel.style.display = 'none';
            horsePowerField.style.display = 'block';
            horsePowerLabel.style.display = 'block';
        } else if (engineTypeSelect.value === '0' && clientTypeSelect.value === '1') { // бенз и юр
            engineCapacityField.style.display = 'block';
            engineCapacityLabel.style.display = 'block';
            horsePowerField.style.display = 'block';
            horsePowerLabel.style.display = 'block';
        } else if (engineTypeSelect.value === '0' && clientTypeSelect.value === '0') { // бенз и физ
            horsePowerField.style.display = 'none';
            horsePowerLabel.style.display = 'none';
            engineCapacityField.style.display = 'block';
            engineCapacityLabel.style.display = 'block';
            
        } 
    }

    // Вызов функции при загрузке страницы
    updateEngineCapacityVisibility();


    // Следим за изменением выбранного типа двигателя
    engineTypeSelect.addEventListener('change', updateEngineCapacityVisibility);
    clientTypeSelect.addEventListener('change', updateEngineCapacityVisibility);
});


/* Форматирование чисел в полях на ВВОДЕ*/
document.addEventListener('DOMContentLoaded', function() {
    const inputs = [
        document.getElementById('carPrice'),
        document.getElementById('engineCapacity'),
        document.getElementById('horsePower')
    ];

    /* добавить корректную обработку курсора */

    function formatNumber(value) {
        const num = value.replace(/\D/g, '');
        return num.replace(/\B(?=(\d{3})+(?!\d))/g, ' ');
    }

    function handleInput(e) {
        const target = e.target;
        const cursorPos = target.selectionStart;
        const formatted = formatNumber(target.value);
        
        target.value = formatted;
    
        const newPos = cursorPos + (formatted.length - target.value.length) + 1;
        target.setSelectionRange(newPos, newPos);
    }

    inputs.forEach(input => {
        if (input) {
            input.addEventListener('input', handleInput);
            // Запрет нечисловых символов (кроме пробелов)
            input.addEventListener('keydown', function(e) {
                if (!/[0-9]|Backspace|Delete|Arrow|Space/.test(e.key)) {
                    e.preventDefault();
                }
            });
        }
    });
});

/* Т.к. форматировать на вводе можно только текст, то приводим тип к численному */
function getNumericValue(inputText) {
    return parseInt(inputText.replace(/\s/g, ''), 10) || 0;
}

/* Форматирование чисел на ВЫВОДЕ */
function formatNumber(number) {
    if (typeof number !== 'number' || isNaN(number)) {
        return '0'; // Если число некорректное, возвращаем 0
    }

    // Форматируем число с пробелами между разрядами
    return number.toLocaleString('ru-RU');
}
/* Округление */
function roundNumber(number) {


    if (number >= 1000000) {
        return number = Math.round(number / 1000) * 1000;
    } else if (number >= 1000) {
        return number = Math.round(number / 100) * 100;
    } else {
        return number = Math.round(number);
    }

}


/* Главная функция по нажатию на кнопку "Рассчитать". Передает данные на бэк и принимает ответ. */
async function calculate() {
    try {
        const carPrice = getNumericValue(document.getElementById('carPrice').value);
        const givenCurrency = document.getElementById('currency').value;
        const carAge = document.getElementById('carAge').value;
        engineCapacity = getNumericValue(document.getElementById('engineCapacity').value);
        horsePower = getNumericValue(document.getElementById('horsePower').value);
        const engineType = document.getElementById('engineType').value;
        const clientType = document.getElementById('clientType').value;

        /* ПРОВЕРКА ВВОДА
        - Стоимость и возраст д.б. заполнены всегда
        - Валюта, тип двигателя и цель ввоза не бывают пустыми
        - ТОЛЬКО Объем двигателя заполняется для:
            - Бенз физ
        - ТОЛЬКО Мощность двигателя заполняется для: 
            - Электро физ
            - Электро юр
        - Объем дв-ля И мощность дв-ля заполняются для
            - Бенз юр*/

        if (!carPrice || !carAge) {
            alert('Пожалуйста, заполните все поля');
            return;
        }
        
        // бензин, только объем, если физ
        if (engineType === '0' && !engineCapacity && clientType === '0') {
            alert('Пожалуйста, введите объем двигателя');
            return;
        }

        // электро, только мощность, юр и физ
        if (engineType === '1' && !horsePower) {
            alert('Пожалуйста, введите мощность двигателя');
            return;
        }
        
        // бензин для юр
        if (engineType === '0' && clientType === '1' && (!horsePower || !engineCapacity)) {
            alert('Пожалуйста, введите объем и мощность двигателя');
            return;
        }

        const engineCapacityValue = engineType === '1' ? 0 : parseInt(engineCapacity);
        

        let horsePowerValue;
        if (engineType === '0' && clientType === '0') {
            horsePowerValue = 0;
        } else if (engineType === '1' || (engineType === '0' && clientType === '1')) {
            horsePowerValue = parseInt(horsePower);
        }

        if(carPrice > 0 && carAge > 0 && (engineCapacity < 10000)) {

            // Отправка данных на сервер
            const response = await fetch('http://localhost:5000/calculate', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    cost: parseFloat(carPrice),
                    age: parseInt(carAge),
                    engine: engineCapacityValue,
                    horsePower: horsePowerValue, 
                    givenCurrency: givenCurrency,
                    engineType: parseInt(engineType),
                    clientType: parseInt(clientType)
                    
                }),
            });

            // Проверка ответа
            if (!response.ok) {
                throw new Error('Ошибка при расчете. Нет ответа от бэка');
            }

            const result = await response.json();
            console.log(result); 
            
            const formattedResult = {
                finalCostRUB: formatNumber(roundNumber(Number(result.finalCostRUB))),
                costInRUB: formatNumber(roundNumber(Number(result.costInRUB))),
                sumFees: formatNumber(roundNumber(Number(result.sumFees))),
                taxFeeRUB: formatNumber(roundNumber(Number(result.taxFeeRUB))),
                taxFeeEUR: formatNumber(Number(result.taxFeeEUR)),
                disposalFeeRUB: formatNumber(Number(result.disposalFeeRUB)),
                customsDutyRUB: formatNumber(Number(result.customsDutyRUB)),
                
                exciseDuty: formatNumber(Number(result.exciseDuty)),
                NDS: formatNumber(Number(result.nds)),
            };
        
            console.log(result)
            console.log(formatNumber(result.finalCostRUB));

        
            /* Подставляем в курс нужный символ валюты */
            let currencySymbol;
            switch(givenCurrency) {
                case 'JPY':
                case 'CNY':
                    currencySymbol = '¥';
                    break;
                case 'KRW':
                    currencySymbol = '₩';
                    break;
                case 'USD':
                    currencySymbol = '$';
                    break;
                default:
                    currencySymbol = '€';
                    break;
            }

            if(result.nds == 0) { // если не было расчета ндс, то выводить ндс и акциз не надо
                
                document.getElementById('result').innerHTML = `
                    <div class="receipt-header">ТАМОЖЕННЫЙ РАСЧЕТ</div>
                    <div class="exchange-rates">
                    <div class="rate-header">Курс ЦБ РФ на ${result.date}:</div>
                        <div class="rates-container">
                            <span class="rate-currency">${currencySymbol} ${result.exchangeGiven}</span>
                            <span class="rate-currency">€ ${result.exchangeEUR}</span>
                        </div>
                    </div>
                    <p>
                        <span class="label">Стоимость авто</span>
                        <span class="value">${formattedResult.costInRUB} ₽</span>
                    </p>
                    <p>
                        <span class="label">Таможенная пошлина ₽</span>
                        <span class="value">${formattedResult.taxFeeRUB} ₽</span>
                    </p>
                    <p>
                        <span class="label" id="euro-rates">Таможенная пошлина €</span>
                        <span class="value" id="euro-rates">${formattedResult.taxFeeEUR} €</span>
                    </p>
                    <p>
                        <span class="label">Утилизационный сбор</span>
                        <span class="value">${formattedResult.disposalFeeRUB} ₽</span>
                    </p>
                    <p>
                        <span class="label">Таможенный сбор</span>
                        <span class="value">${formattedResult.customsDutyRUB} ₽</span>
                    </p>
                        
                    <p class="total-line">
                        <span class="label">ИТОГО</span>
                        <span class="value">${formattedResult.finalCostRUB} ₽</span>
                    </p>
                
                `;

            } else {
                
                document.getElementById('result').innerHTML = `
                    <div class="receipt-header">ТАМОЖЕННЫЙ РАСЧЕТ</div>
    
                    <p>
                        <span class="label">Стоимость авто</span>
                        <span class="value">${formattedResult.costInRUB} ₽</span>
                    </p>
                    <p>
                        <span class="label">Таможенная пошлина</span>
                        <span class="value">${formattedResult.taxFeeRUB} ₽</span>
                    </p>
                    <p>
                        <span class="label">Утилизационный сбор</span>
                        <span class="value">${formattedResult.disposalFeeRUB} ₽</span>
                    </p>
                    <p>
                        <span class="label">Таможенный сбор</span>
                        <span class="value">${formattedResult.customsDutyRUB} ₽</span>
                    </p>
                    <p>
                        <span class="label">Акциз</span>
                        <span class="value">${formattedResult.exciseDuty} ₽</span>
                    </p>
                    <p>
                        <span class="label">НДС</span>
                        <span class="value">${formattedResult.NDS} ₽</span>
                    </p>
                    
                    <p class="total-line">
                        <span class="label">ИТОГО</span>
                        <span class="value">${formattedResult.finalCostRUB} ₽</span>
                    </p>
                `;
            }
            
    }
    else alert('Пожалуйста, проверьте правильность заполнения');
    } catch (error) {
        document.getElementById('result').innerText = `Ошибка: ${error.message}`;
    }
}