from flask import Flask, request, jsonify
from flask_cors import CORS
import ctypes
from datetime import datetime
import os
import requests

app = Flask(__name__)
CORS(app) 

# Определяем структуру для результата
class CustomsResult(ctypes.Structure):
    _fields_ = [
        ("finalCostRUB", ctypes.c_int),
        ("costInRUB", ctypes.c_int),
        ("sumFees", ctypes.c_int),
        ("taxFeeRUB", ctypes.c_int),
        ("taxFeeEUR", ctypes.c_int),
        ("disposalFeeRUB", ctypes.c_int),
        ("customsDutyRUB", ctypes.c_int),
        ("exciseDuty", ctypes.c_int),
        ("nds", ctypes.c_int),
    ]

url = "https://www.cbr-xml-daily.ru/daily_json.js"

response = requests.get(url)

def get_date():
    if response.status_code == 200:
        # Парсим JSON
        data = response.json()
        return data["Date"]
    else:
        raise Exception(f"Ошибка при запросе к API: {response.status_code}")

def get_currency_rates():
    
    if response.status_code == 200:
        # Парсим JSON
        data = response.json()
        return data["Valute"]
    else:
        raise Exception(f"Ошибка при запросе к API: {response.status_code}")
        

def get_currency_value(currency, valute):
    if currency in valute:
        findCurrency = valute[currency]
        value = findCurrency['Value']
        print(f"Курс обмена {currency}: {value}")
        return value
    else:
        return jsonify({"error": "Валюта не найдена"}), 400

def get_nominal(currency, valute):
    if currency in valute:
         findCurrency = valute[currency]
         nominal = findCurrency['Nominal']
         print(f"Номинал: {nominal}")
         return nominal
    else:
        return jsonify({"error": "Валюта не найдена"}), 400



# Загрузка shared library
dll_path = os.path.join(os.path.dirname(__file__), "") # полный путь до /backend/build/Debug/calc.dll
lib = ctypes.CDLL(dll_path)


# Что передается в функцию
lib.calculateCustomsFee.argtypes = [
    ctypes.c_double, # costInGivenCurrency
    ctypes.c_int, # carAge
    ctypes.c_int, # engineCapacity
    ctypes.c_int, # horsePower
    ctypes.c_int, # engineType
    ctypes.c_int, # clientType
    ctypes.c_double, # exchangeRateGivenCurrency
    ctypes.c_double, # exchangeRateEUR
    ctypes.c_int # nominal
]

lib.calculateCustomsFee.restype = CustomsResult


@app.route('/calculate', methods=['POST'])
def calculate():
    data = request.json
    cost = data['cost']
    age = data['age']
    engine = data['engine'] #engineVolume
    engineType = data['engineType'] #engineType
    horsePower = data['horsePower']
    client = data['clientType']
    exchangeGivenCurrency = data['givenCurrency'].encode('utf-8')

    date = get_date()

    date_obj = datetime.fromisoformat(date)

    # Форматирование даты
    formatted_date = date_obj.strftime("%d.%m.%Y")

    valute = get_currency_rates()
    givenCurrencyStr = exchangeGivenCurrency.decode('utf-8')  # bytes в строку
    print(f"Запрошенная валюта: {givenCurrencyStr}")
    
    print(f'Тип двигателя: {engineType}')
    print(f'Цель ввоза: {client}')
    print(f'Мощность: {horsePower}')
    

    exchangeGiven = get_currency_value(givenCurrencyStr, valute)
    nominal = get_nominal(givenCurrencyStr, valute)
    exchangeEUR = get_currency_value("EUR", valute)

    result = lib.calculateCustomsFee(
        ctypes.c_double(cost), 
        ctypes.c_int(age), 
        ctypes.c_int(engine), 
        ctypes.c_int(horsePower), 
        ctypes.c_int(engineType), 
        ctypes.c_int(client), 
        ctypes.c_double(exchangeGiven),
        ctypes.c_double(exchangeEUR),
        ctypes.c_int(nominal)
    )


    print(f'Акциз: {result.exciseDuty}')
    print(f'Стоимость в руб: {result.costInRUB}')
    
    print(f"Результат: {result}")
    
    exchangeGiven = round(exchangeGiven, 2)
    exchangeEUR = round(exchangeEUR, 2)

    return jsonify({
        "finalCostRUB": result.finalCostRUB,
        "costInRUB": result.costInRUB, # без пошлин из исходной валюты
        "sumFees": result.sumFees, # только в рублях
        "taxFeeRUB": result.taxFeeRUB,
        "taxFeeEUR": result.taxFeeEUR,
        "disposalFeeRUB": result.disposalFeeRUB,
        "customsDutyRUB": result.customsDutyRUB,
        "exciseDuty": result.exciseDuty,
        "nds": result.nds,
        "exchangeGiven": exchangeGiven,
        "exchangeEUR": exchangeEUR,
        "date": formatted_date
    })
    
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
