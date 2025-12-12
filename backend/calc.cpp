#include "calc.h"
#include <string>
#include <cmath>



int calculateFeeForCarUnder3Years(
    int engineCapacity, 
    int costInEuro);

int calculateFeeForCar3To5Years(int engineCapacity);

int calculateFeeForCarOver5Years(int engineCapacity);

int calculateDisposalFee(int carAge, int engineCapacity, int engineType, int clientType);

int calculateCustomsDuty(int costInRubles);

int calculateExciseDuty(int horsePower);

int convertGivenCurrencyToEUR(
    double exchangeRateEUR, 
    int costInGivenCurrency,
    double exchangeRateGivenCurrency,
    int nominal);

int convertGivenCurrencyToRUB(
    double exchangeRateGivenCurrency, 
    double costInGivenCurrency,
    int nominal);

int convertEURToRUB(int taxFeeEUR, double exchangeRateRUB);

int calculateFeeForElectro(int costInRUB);

int calculateNDS(int sumCostInRUB);

/* типа main */
extern "C" __declspec(dllexport) CustomsResult calculateCustomsFee(
    double costInGivenCurrency, 
    int carAge,
    int engineCapacity,
    int horsePower,
    int engineType, 
    int clientType,
    double exchangeRateGivenCurrency, 
    double exchangeRateEUR,
    int nominal) {
        
    CustomsResult result; 

    /* КОНВЕРТАЦИЯ ОТНОСИТЕЛЬНО РУБЛЯ */
    int rateRUB = 1;
    
    /* введенная валюта в рубли */
    result.costInRUB = convertGivenCurrencyToRUB(
        exchangeRateGivenCurrency, 
        costInGivenCurrency,
        nominal);

    /* ТАМОЖЕННАЯ ПОШЛИНА */    
    /* Т. пошлина физ/юр бензин/дизель/параллельные гибриды */
    if(engineType == 0) {
        
        // рубли в евро
        int costInEuro = convertGivenCurrencyToEUR(exchangeRateEUR, costInGivenCurrency, exchangeRateGivenCurrency, nominal);

        /* таможенная пошлина в EUR */
        if (carAge <= 3)
        {   
            result.taxFeeEUR = calculateFeeForCarUnder3Years(engineCapacity, costInEuro);
        }
        else if (carAge > 3 && carAge <= 5)
        {
            result.taxFeeEUR = calculateFeeForCar3To5Years(engineCapacity);
        }
        else if (carAge > 5)
        {
            result.taxFeeEUR = calculateFeeForCarOver5Years(engineCapacity);
        }

        /* таможенная пошлина в RUB */
        result.taxFeeRUB = convertEURToRUB(result.taxFeeEUR, exchangeRateEUR);
    } 
    else /* Т. пошлина электро / последовательные гибриды */
    {
        result.taxFeeRUB = calculateFeeForElectro(result.costInRUB);
    }

    /* утильсбор рассчитывается из стоимости в рублях */ 
    result.disposalFeeRUB = calculateDisposalFee(carAge, engineCapacity, engineType, clientType);

    /* таможенный сбор единый. рассчитывается из стоимости в рублях */
    result.customsDutyRUB = calculateCustomsDuty(result.costInRUB);

    /* Акциз */
    // если бенз + физ - false
    if(engineType == 1 || clientType == 1) {
        result.exciseDuty = calculateExciseDuty(horsePower);
        result.NDS = calculateNDS(result.costInRUB + result.exciseDuty + result.taxFeeRUB);   
        result.taxFeeEUR = 0;
    }
    else
    {
        result.exciseDuty = NULL;
        result.NDS = NULL;
    }
    
    /* Сумма всех сборов и пошлин */
    result.sumFees = result.taxFeeRUB + result.disposalFeeRUB + result.customsDutyRUB + result.exciseDuty + result.NDS;

    /*итоговая стоимость с пошлиной из евро в рубли */
    result.finalCostRUB = result.sumFees + result.costInRUB;

    return result;

}

/* Т. пошлина бензин / дизель / параллельные гибриды ДО 3 лет*/
int calculateFeeForCarUnder3Years(int engineCapacity, int costInEuro) {
    int percent;
    double coefficient;

    if (costInEuro <= 8500.)
    {
        percent = 54;
        coefficient = 2.5;
    }
    else if (costInEuro > 8500. && costInEuro <= 16700.)
    {
        percent = 48;
        coefficient = 3.5;
    }
    else if (costInEuro > 16700. && costInEuro <= 42300.)
    {
        percent = 48;
        coefficient = 5.5;
    }
    else if (costInEuro > 42300. && costInEuro <= 84500.)
    {
        percent = 48;
        coefficient = 7.5;
    }
    else if (costInEuro > 84500. && costInEuro <= 169000.)
    {
        percent = 48;
        coefficient = 15;
    }
    else if (costInEuro > 169000.)
    {
        percent = 48;
        coefficient = 20;
    }
    int feeFromPercent = round(costInEuro * percent * 0.01);
    int feeFromCoefficient = round(coefficient * engineCapacity);

    return (feeFromPercent >= feeFromCoefficient)
           ? feeFromPercent
           : feeFromCoefficient;

}

/* Т. пошлина бензин / дизель / параллельные гибриды ОТ 3 ДО 5 лет*/
int calculateFeeForCar3To5Years(int engineCapacity)
{
    double coefficient = 0;
    
    if (engineCapacity <= 1000)
        coefficient = 1.5;
    else if (engineCapacity > 1000 && engineCapacity <= 1500)
        coefficient = 1.7;
    else if (engineCapacity > 1500 && engineCapacity <= 1800)
        coefficient = 2.5;
    else if (engineCapacity > 1800 && engineCapacity <= 2300)
        coefficient = 2.7;
    else if (engineCapacity > 2300 && engineCapacity <= 3000)
        coefficient = 3.;
    else if (engineCapacity > 3000)
        coefficient = 3.6;

    int result = round(coefficient * engineCapacity);

    return result;
}

/* Т. пошлина бензин / дизель / параллельные гибриды БОЛЕЕ 5 лет*/
int calculateFeeForCarOver5Years(int engineCapacity)
{
    double coefficient = 0;

    if (engineCapacity <= 1000)
        coefficient = 3.;
    else if (engineCapacity > 1000 && engineCapacity <= 1500)
        coefficient = 3.2;
    else if (engineCapacity > 1500 && engineCapacity <= 1800)
        coefficient = 3.5;
    else if (engineCapacity > 1800 && engineCapacity <= 2300)
        coefficient = 4.8;
    else if (engineCapacity > 2300 && engineCapacity <= 3000)
        coefficient = 5.;
    else if (engineCapacity > 3000)
        coefficient = 5.7;

    int result = round(coefficient * engineCapacity);

    return result;
}

/* Т. пошлина электро 15% от стоимости руб */
int calculateFeeForElectro(int costInRUB) {
    
    int result = round(costInRUB * 0.15);
    return result;
}

/* НДС 20% электро и юр лица*/

/* ИСПРАВИТЬ! НДС считает от суммы пошлин (каких?) и стомости товара 
Налог исчисляется путём 
сложения сумм оплаченного акциза, таможенной пошлины и стоимости машины.
*/
int calculateNDS(int sumCostInRUB) {

    int result = round(sumCostInRUB * 0.20);
    return result;
}

/* Акциз для физ (электро)  и юр (все) */
int calculateExciseDuty(int horsePower){

   /* функция */
   int exciseDutyRUB;

    if(horsePower < 90) {
        exciseDutyRUB = 0;
    } else if(horsePower > 90 && horsePower <= 150) {
        exciseDutyRUB = 61;
    } else if(horsePower > 150 && horsePower <= 200) {
        exciseDutyRUB = 583;
    } else if(horsePower > 200 && horsePower <= 300) {
        exciseDutyRUB = 955;
    } else if(horsePower > 300 && horsePower <= 400) {
        exciseDutyRUB = 1628;
    } else if(horsePower > 400 && horsePower <= 500) {
        exciseDutyRUB = 1685;
    } else if(horsePower > 500) {
        exciseDutyRUB = 1740;
    }

    return horsePower * exciseDutyRUB;
}

/* сверка данных */   
/* Утиль для всех */
int calculateDisposalFee(int carAge, int engineCapacity, int engineType, int clientType)
{
    int disposalFee = 1;

    if(engineType == 0) { // бенз

        if(clientType == 0) // физ
        {    
            if (carAge <= 3) 
            {
                if (engineCapacity <= 3000) // проверить!!!
                    disposalFee = 3400;
                else if (engineCapacity <= 3500)
                    disposalFee = 2153400;
            } 
            else
            {
                if (engineCapacity <= 3000)
                    disposalFee = 5200;
                else if (engineCapacity <= 3500)
                    disposalFee = 3296800;
            }

        } else if(clientType == 1) { // юр
            if (carAge <= 3) 
            {
                if (engineCapacity <= 1000)
                    disposalFee = 180200;
                else if (engineCapacity > 1000 && engineCapacity <= 2000)
                    disposalFee = 667400;
                else if (engineCapacity > 2000 && engineCapacity <= 3000)
                    disposalFee = 1875400;
                else if (engineCapacity > 3000 && engineCapacity <= 3500)
                    disposalFee = 2153400;
                else if(engineCapacity > 3500)
                    disposalFee = 2742200;
            } 
            else if(carAge > 3)
            {
                if (engineCapacity <= 1000)
                    disposalFee = 460000;
                else if (engineCapacity > 1000 && engineCapacity <= 2000)
                    disposalFee = 1174000;
                else if (engineCapacity > 2000 && engineCapacity <= 3000)
                    disposalFee = 2839400;
                else if (engineCapacity > 3000 && engineCapacity <= 3500)
                    disposalFee = 3296800;
                else if(engineCapacity > 3500)
                    disposalFee = 3604800;
            }
        }

    } else if(engineType == 1) { // электро
            
        if(clientType == 0) // физ
        {
            if(carAge > 0 && carAge <= 3)
                disposalFee = 3400; // в структуру result 
            else if(carAge > 3)
                disposalFee = 5200;
                    
        } else if(clientType == 1) { // юр
            if(carAge > 0 && carAge <= 3)
                disposalFee = 674000; // в структуру result 
            else if(carAge > 3)
                disposalFee = 1174000;
        }

    }

    return disposalFee;
}

int calculateCustomsDuty(int costInRubles)
{
    int customsDuty = 1;

    if (costInRubles <= 200000)
        customsDuty = 1067;
    else if (costInRubles > 200000 && costInRubles <= 450000)
        customsDuty = 2134;
    else if (costInRubles > 450000 && costInRubles <= 1200000)
        customsDuty = 4269;
    else if (costInRubles > 1200000 && costInRubles <= 2700000)
        customsDuty = 11746;
    else if (costInRubles > 2700000 && costInRubles <= 4200000)
        customsDuty = 16524;
    else if (costInRubles > 4200000 && costInRubles <= 5500000)
        customsDuty = 21344;
    else if (costInRubles > 5500000 && costInRubles <= 7000000)
        customsDuty = 27540;
    else if (costInRubles > 7000000)
        customsDuty = 30000;

    return customsDuty;
}

int convertGivenCurrencyToEUR(
    double exchangeRateEUR, 
    int costInGivenCurrency,
    double exchangeRateGivenCurrency,
    int nominal)
{
    int result = round((costInGivenCurrency * (exchangeRateGivenCurrency / nominal)) / exchangeRateEUR);
    
    return result;
}

int convertGivenCurrencyToRUB(
    double exchangeRateGivenCurrency, 
    double costInGivenCurrency,
    int nominal)
{
    int result = round((exchangeRateGivenCurrency / nominal) * costInGivenCurrency);

    return result;
}

int convertEURToRUB(int taxFeeEUR, double exchangeRateEUR)
{
    int result = round(taxFeeEUR * exchangeRateEUR);
    
    return result; 
}