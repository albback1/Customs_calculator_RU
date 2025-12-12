#ifndef CALC_H
#define CALC_H

#ifdef __cplusplus
extern "C" {
#endif

struct CustomsResult {
    
    int finalCostRUB; // RUB сумма costInRUB и sumFees +
    int costInRUB; // RUB без пошлин +
    int sumFees; // RUB сумма  taxFeeRUB, disposalFeeRUB, customsDuty +
    int taxFeeRUB; // RUB +
    int taxFeeEUR; // EUR +
    int disposalFeeRUB; // RUB +
    int customsDutyRUB; // RUB +
    int exciseDuty; // RUB -
    int NDS; // RUB -
};

__declspec(dllexport) CustomsResult calculateCustomsFee(
    double costInGivenCurrency, 
    int carAge,
    int engineCapacity,
    int horsePower,
    int engineType, 
    int clientType,
    double exchangeRateGivenCurrency, 
    double exchangeRateEUR,
    int nominal);

#ifdef __cplusplus
}
#endif

#endif // CALC_H
