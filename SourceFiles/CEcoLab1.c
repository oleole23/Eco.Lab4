/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов CEcoLab1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab1.h"
#include "CEcoLab1EnumConnectionPoints.h"
#include "IEcoConnectionPointContainer.h"
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"
#include "Scheduler.h"

/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoLab1
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1_QueryInterface(/* in */ struct IEcoLab1* me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателей */
    if (me == 0 || ppv == 0) {
        return -1;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoLab1) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
	else if ( IsEqualUGUID(riid, &IID_IEcoConnectionPointContainer) ) {
        *ppv = &pCMe->m_pVTblICPC;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX) && pCMe->m_pInnerUnknown != 0) {
        return pCMe->m_pInnerUnknown->pVTbl->QueryInterface(pCMe->m_pInnerUnknown, riid, ppv);
    } 
	else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
	else if (IsEqualUGUID(riid, &IID_IEcoCalculatorY)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
	else if (IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
	else {
        *ppv = 0;
        return -1;
    }
    return 0;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoLab1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoLab1_AddRef(/* in */ struct IEcoLab1* me) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return -1;
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoLab1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoLab1_Release(/* in */ struct IEcoLab1* me) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    --pCMe->m_cRef;

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoLab1((IEcoLab1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}


void sortingArrivalTime(struct Process processes[], int n) {
    int i, j;
    struct Process temp;

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

#define MAX 100
int16_t ECOCALLMETHOD CEcoLab1_scheduleProcesses(struct IEcoLab1* me, struct Process processes[], int n, int *completion_order) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
	int time = 0;  
    int completed = 0;
    int is_completed[MAX];
	int i, t, j;
	int index = 0;
	int process_found = 0;

    for (i = 0; i < n; i++) {
        is_completed[i] = 0;
    }   

	sortingArrivalTime(processes, n);

    while (completed < n) {
		process_found = 0;
        for (i = 0; i < n; i++) {
            if (processes[i].arrival_time <= time && !is_completed[i]) {
				processes[i].waiting_time = time - processes[i].arrival_time;

                for (t = 0; t < processes[i].burst_time; t++) {
                    time++;
                }
				processes[i].completion_time = time;
                is_completed[i] = 1;
                completion_order[index++] = processes[i].id;
                completed++;
				process_found = 1;
            }
        }
		if (!process_found) {
            time++;
		}
	}
}

/*
 *
 * <сводка>
 *   Функция Fire_OnMyCallback
 * </сводка>
 *
 * <описание>
 *   Функция вызова обратного интерфейса
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1_Fire_OnBeforeBucketSort(/* in */ struct IEcoLab1* me, const void * array, size_t count) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    int16_t result = 0;
    IEcoEnumConnections* pEnum = 0;
    IEcoLab1Events* pIEvents = 0;
    EcoConnectionData cd;

    if (me == 0 ) {
        return -1;
    }

    if (pCMe->m_pISinkCP != 0) {
        result = ((IEcoConnectionPoint*)pCMe->m_pISinkCP)->pVTbl->EnumConnections((IEcoConnectionPoint*)pCMe->m_pISinkCP, &pEnum);
        if ( (result == 0) && (pEnum != 0) ) {
            while (pEnum->pVTbl->Next(pEnum, 1, &cd, 0) == 0) {
                result = cd.pUnk->pVTbl->QueryInterface(cd.pUnk, &IID_IEcoLab1Events, (void**)&pIEvents);
                if ( (result == 0) && (pIEvents != 0) ) {
                    result = pIEvents->pVTbl->OnBeforeBucketSort(pIEvents, array, count);
                    pIEvents->pVTbl->Release(pIEvents);
                }
                cd.pUnk->pVTbl->Release(cd.pUnk);
            }
            pEnum->pVTbl->Release(pEnum);
        }
    }
    return result;
}

int16_t ECOCALLMETHOD CEcoLab1_Fire_BucketSortStep(/* in */ struct IEcoLab1* me, const void *bucket, size_t bucket_size, int16_t bucket_index) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    int16_t result = 0;
    IEcoEnumConnections* pEnum = 0;
    IEcoLab1Events* pIEvents = 0;
    EcoConnectionData cd;

    if (me == 0 ) {
        return -1;

    }

    if (pCMe->m_pISinkCP != 0) {
        result = ((IEcoConnectionPoint*)pCMe->m_pISinkCP)->pVTbl->EnumConnections((IEcoConnectionPoint*)pCMe->m_pISinkCP, &pEnum);
        if ( (result == 0) && (pEnum != 0) ) {
            while (pEnum->pVTbl->Next(pEnum, 1, &cd, 0) == 0) {
                result = cd.pUnk->pVTbl->QueryInterface(cd.pUnk, &IID_IEcoLab1Events, (void**)&pIEvents);
                if ( (result == 0) && (pIEvents != 0) ) {
                    result = pIEvents->pVTbl->BucketSortStep(pIEvents, bucket, bucket_size, bucket_index);
                    pIEvents->pVTbl->Release(pIEvents);
                }
                cd.pUnk->pVTbl->Release(cd.pUnk);
            }
            pEnum->pVTbl->Release(pEnum);
        }
    }
    return result;
}


int16_t ECOCALLMETHOD CEcoLab1_Fire_OnAfterBucketSort(/* in */ struct IEcoLab1* me, const void * array, size_t count) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    int16_t result = 0;
    IEcoEnumConnections* pEnum = 0;
    IEcoLab1Events* pIEvents = 0;
    EcoConnectionData cd;

    if (me == 0 ) {
        return -1;

    }

    if (pCMe->m_pISinkCP != 0) {
        result = ((IEcoConnectionPoint*)pCMe->m_pISinkCP)->pVTbl->EnumConnections((IEcoConnectionPoint*)pCMe->m_pISinkCP, &pEnum);
        if ( (result == 0) && (pEnum != 0) ) {
            while (pEnum->pVTbl->Next(pEnum, 1, &cd, 0) == 0) {
                result = cd.pUnk->pVTbl->QueryInterface(cd.pUnk, &IID_IEcoLab1Events, (void**)&pIEvents);
                if ( (result == 0) && (pIEvents != 0) ) {
                    result = pIEvents->pVTbl->OnAfterBucketSort(pIEvents, array, count);
                    pIEvents->pVTbl->Release(pIEvents);
                }
                cd.pUnk->pVTbl->Release(cd.pUnk);
            }
            pEnum->pVTbl->Release(pEnum);
        }
    }
    return result;
}

/*
 *
 * <сводка>
 *   Функция MyFunction
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
#define N 10000
int16_t ECOCALLMETHOD CEcoLab1_BucketSort(/* in */ struct IEcoLab1* me, /* in */ void *base, size_t n, int16_t max_val, int (__cdecl *compare) (const void *, const void *)) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
	size_t i;
	size_t j;
	size_t s;
	int16_t index;
	int16_t* array = (int16_t*)base;

	int16_t (*positive_buckets)[N] = (int16_t(*)[N])pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, max_val * N * sizeof(int16_t));
	size_t* positive_bucket_sizes = (size_t*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, max_val * sizeof(size_t));

	int16_t (*negative_buckets)[N] = (int16_t(*)[N])pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, max_val * N * sizeof(int16_t));
	size_t* negative_bucket_sizes = (size_t*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, max_val * sizeof(size_t));

    /* Проверка указателей */
    if (me == 0) {
        return -1;
    }

	CEcoLab1_Fire_OnBeforeBucketSort(me, array, n);

	max_val++;
	
    for (i = 0; i < max_val; ++i) {
        positive_bucket_sizes[i] = 0;
        negative_bucket_sizes[i] = 0;
    }

    for (j = 0; j < n; ++j) {
        index = array[j];
		if (index >= 0) {
			positive_buckets[index][positive_bucket_sizes[index]++] = array[j];
		} else {
			negative_buckets[-index][negative_bucket_sizes[-index]++] = -array[j];
		}
    }

	for (s = 0; s < max_val; ++s) {
		if (positive_bucket_sizes[s] > 0) {
            int16_t* positive_bucket = positive_buckets[s];
            size_t positive_size = positive_bucket_sizes[s];

			CEcoLab1_Fire_BucketSortStep(me, positive_buckets[s], positive_bucket_sizes[s], s);

			for (j = 1; j < positive_size; ++j) {
                int16_t key = positive_bucket[j];
                int16_t k = j - 1;
				
                while (k >= 0 && compare(&positive_bucket[k], &key) > 0) {
                    positive_bucket[k + 1] = positive_bucket[k];
                    k = k - 1;
                }

                positive_bucket[k + 1] = key;
            }
        }
	}

	for (s = max_val - 1; s != 0; --s) {
		if (negative_bucket_sizes[s] > 0) {
            int16_t* negative_bucket = negative_buckets[s];
            size_t negative_size = negative_bucket_sizes[s];

			CEcoLab1_Fire_BucketSortStep(me, negative_buckets[s], negative_bucket_sizes[s], -s);

            for (j = 1; j < negative_size; ++j) {
                int16_t key = negative_bucket[j];
                int16_t k = j - 1;

                while (k >= 0 && compare(&negative_bucket[k], &key) < 0) {
                    negative_bucket[k + 1] = negative_bucket[k];
                    k = k - 1;
                }

                negative_bucket[k + 1] = key;
            }
        }
    }

	index = 0;
    for (i = max_val-1; i != 0; --i) {
		for (j = 0; j < negative_bucket_sizes[i]; ++j) {
			array[index++] = -negative_buckets[i][j];
		}
	}

	for (i = 0; i < max_val; ++i) {
		for (j = 0; j < positive_bucket_sizes[i]; ++j) {
            array[index++] = positive_buckets[i][j];
        }
    }

	CEcoLab1_Fire_OnAfterBucketSort(me, array, n);

	pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, positive_buckets);
    pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, negative_buckets);

    return 0;
}


int16_t CEcoLab1_IEcoCalculatorX_QueryInterface(/* in */ struct IEcoCalculatorX* me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 2);

    /* Проверка указателей */
    if (me == 0 || ppv == 0) {
        return -1;
    }

    /* Проверка и получение запрошенного интерфейса */
    if (IsEqualUGUID(riid, &IID_IEcoLab1)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
	else if (IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
	else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX) && pCMe->m_pInnerUnknown != 0) {
        return pCMe->m_pInnerUnknown->pVTbl->QueryInterface(pCMe->m_pInnerUnknown, riid, ppv);
    } 
	else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
	else if (IsEqualUGUID(riid, &IID_IEcoCalculatorY)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
	else {
        *ppv = 0;
        return -1;
    }
    return 0;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoCalculatorX
 * </описание>
 *
 */
uint32_t CEcoLab1_IEcoCalculatorX_AddRef(/* in */ struct IEcoCalculatorX* me) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 2);

    /* Проверка указателя */
    if (me == 0) {
        return -1;
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoCalculatorX
 * </описание>
 *
 */
uint32_t CEcoLab1_IEcoCalculatorX_Release(/* in */ struct IEcoCalculatorX* me) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 2);

    /* Проверка указателя */
    if (me == 0) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    --pCMe->m_cRef;

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if (pCMe->m_cRef == 0) {
        deleteCEcoLab1((IEcoLab1*) pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}


int32_t CEcoLab1_Addition(/* in */ struct IEcoCalculatorX* me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 2);

    /* Проверка указателей */
    if (me == 0 || pCMe->m_pIEcoCalculatorX == 0) {
        return -1;
    }

    return pCMe->m_pIEcoCalculatorX->pVTbl->Addition(pCMe->m_pIEcoCalculatorX, a, b);
}

int16_t CEcoLab1_Subtraction(/* in */ struct IEcoCalculatorX* me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 2);

    /* Проверка указателей */
    if (me == 0 || pCMe->m_pIEcoCalculatorX == 0) {
        return -1;
    }

    return pCMe->m_pIEcoCalculatorX->pVTbl->Subtraction(pCMe->m_pIEcoCalculatorX, a, b);
}

int16_t CEcoLab1_IEcoCalculatorY_QueryInterface(/* in */ struct IEcoCalculatorY* me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 3);

    /* Проверка указателей */
    if (me == 0 || ppv == 0) {
        return -1;
    }

    /* Проверка и получение запрошенного интерфейса */
    if (IsEqualUGUID(riid, &IID_IEcoLab1)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } else if (IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
    } else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX) && pCMe->m_pInnerUnknown != 0) {
        return pCMe->m_pInnerUnknown->pVTbl->QueryInterface(pCMe->m_pInnerUnknown, riid, ppv);
    } else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } else if (IsEqualUGUID(riid, &IID_IEcoCalculatorY)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } else {
        *ppv = 0;
        return -1;
    }
    return 0;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoCalculatorY
 * </описание>
 *
 */
uint32_t CEcoLab1_IEcoCalculatorY_AddRef(/* in */ struct IEcoCalculatorY* me) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 3);

    /* Проверка указателя */
    if (me == 0) {
        return -1;
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoCalculatorY
 * </описание>
 *
 */
uint32_t CEcoLab1_IEcoCalculatorY_Release(/* in */ struct IEcoCalculatorY* me) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 3);

    /* Проверка указателя */
    if (me == 0) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    --pCMe->m_cRef;

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if (pCMe->m_cRef == 0) {
        deleteCEcoLab1((IEcoLab1*) pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

int32_t CEcoLab1_Multiplication(/* in */ struct IEcoCalculatorY* me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 3);

    if (me == 0 || pCMe->m_pIEcoCalculatorY == 0) {
        return -1;
    }

    return pCMe->m_pIEcoCalculatorY->pVTbl->Multiplication(pCMe->m_pIEcoCalculatorY, a, b);
}


int16_t CEcoLab1_Division(/* in */ struct IEcoCalculatorY* me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 3);

    if (me == 0 || pCMe->m_pIEcoCalculatorY == 0) {
        return -1;
    }

    return pCMe->m_pIEcoCalculatorY->pVTbl->Division(pCMe->m_pIEcoCalculatorY, a, b);
}


/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoConnectionPointContainer
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1_IEcoConnectionPointContainer_QueryInterface(/* in */ struct IEcoConnectionPointContainer* me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoUnknown*));

    if (me == 0 || ppv == 0) {
        return -1;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoLab1) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoConnectionPointContainer) ) {
        *ppv = &pCMe->m_pVTblICPC;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else {
        *ppv = 0;
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoConnectionPointContainer
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoLab1_IEcoConnectionPointContainer_AddRef(/* in */ struct IEcoConnectionPointContainer* me) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoUnknown*));

    if (me == 0 ) {
        return -1;
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoConnectionPointContainer
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoLab1_IEcoConnectionPointContainer_Release(/* in */ struct IEcoConnectionPointContainer* me) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoUnknown*));

    if (me == 0 ) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    --pCMe->m_cRef;

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoLab1((IEcoLab1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция EnumConnectionPoints
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1_IEcoConnectionPointContainer_EnumConnectionPoints(/* in */ struct IEcoConnectionPointContainer* me, /* out */ struct IEcoEnumConnectionPoints **ppEnum) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoUnknown*));
    int16_t result = 0;

    if (me == 0 || ppEnum == 0 ) {
        return -1;
    }

    result = createCEcoLab1EnumConnectionPoints((IEcoUnknown*)pCMe->m_pISys, &pCMe->m_pISinkCP->m_pVTblICP, ppEnum);

    return result;
}

/*
 *
 * <сводка>
 *   Функция FindConnectionPoint
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1_IEcoConnectionPointContainer_FindConnectionPoint(/* in */ struct IEcoConnectionPointContainer* me, /* in */ const UGUID* riid, /* out */ struct IEcoConnectionPoint** ppCP) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoUnknown*)*3);
    int16_t result = 0;

    if (me == 0 || ppCP == 0) {
        return -1;
    }

    if (!IsEqualUGUID(riid, &IID_IEcoLab1Events)) {
        *ppCP = 0;
        /* CONNECT_E_NOCONNECTION */
        return -1;
    }

    if (pCMe->m_pISinkCP == 0) {
        /* E_FAIL */
        return -1;
    }

    pCMe->m_pISinkCP->m_pVTblICP->AddRef(&pCMe->m_pISinkCP->m_pVTblICP);
    *ppCP = &pCMe->m_pISinkCP->m_pVTblICP;

    return 0;
}



/*
 *
 * <сводка>
 *   Функция Init
 * </сводка>
 *
 * <описание>
 *   Функция инициализации экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD initCEcoLab1(/*in*/ struct IEcoLab1* me, /* in */ struct IEcoUnknown *pIUnkSystem) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    IEcoInterfaceBus1* pIBus = 0;
	IEcoUnknown* pOuterUnknown = (IEcoUnknown*)me;
    int16_t result = -1;

    /* Проверка указателей */
    if (me == 0 ) {
        return result;
    }

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    /* Проверка указателей */
    if (me == 0 ) {
        return result;
    }

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    /* Создание точки подключения */
    result = createCEcoLab1ConnectionPoint((IEcoUnknown*)pCMe->m_pISys, &pCMe->m_pVTblICPC, &IID_IEcoLab1Events, (IEcoConnectionPoint**)&((pCMe)->m_pISinkCP));
    if (result == 0 && pCMe->m_pISinkCP != 0) {
        result = 0;
    }

	result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, pOuterUnknown, &IID_IEcoUnknown, (void**) &pCMe->m_pInnerUnknown);
    if (result != 0 || pCMe->m_pInnerUnknown == 0) {
        result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorA, 0, &IID_IEcoCalculatorX, (void**) &pCMe->m_pIEcoCalculatorX);
    }
    if (result != 0) {
        goto error;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, &IID_IEcoCalculatorY, (void**) &pCMe->m_pIEcoCalculatorY);
    if (result != 0 || pCMe->m_pIEcoCalculatorY == 0) {
        result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, &IID_IEcoCalculatorY, (void**) &pCMe->m_pIEcoCalculatorY);
    }

error:

    /* Освобождение */
    pIBus->pVTbl->Release(pIBus);

    return result;
}

/* Create Virtual Table IEcoLab1 */
IEcoLab1VTbl g_x277FC00C35624096AFCFC125B94EEC90VTbl = {
    CEcoLab1_QueryInterface,
    CEcoLab1_AddRef,
    CEcoLab1_Release,
    CEcoLab1_BucketSort,
	CEcoLab1_scheduleProcesses
};

/* Create Virtual Table IEcoConnectionPointContainer */
IEcoConnectionPointContainerVTbl g_x0000000500000000C000000000000046VTblCPC = {
    CEcoLab1_IEcoConnectionPointContainer_QueryInterface,
    CEcoLab1_IEcoConnectionPointContainer_AddRef,
    CEcoLab1_IEcoConnectionPointContainer_Release,
    CEcoLab1_IEcoConnectionPointContainer_EnumConnectionPoints,
    CEcoLab1_IEcoConnectionPointContainer_FindConnectionPoint
};

IEcoCalculatorXVTbl g_x9322111622484742AE0682819447843DVTblCX = {
    CEcoLab1_IEcoCalculatorX_QueryInterface,
    CEcoLab1_IEcoCalculatorX_AddRef,
    CEcoLab1_IEcoCalculatorX_Release,
    CEcoLab1_Addition,
    CEcoLab1_Subtraction
};

IEcoCalculatorYVTbl g_xBD6414C29096423EA90C04D77AFD1CADVTblCY = {
    CEcoLab1_IEcoCalculatorY_QueryInterface,
    CEcoLab1_IEcoCalculatorY_AddRef,
    CEcoLab1_IEcoCalculatorY_Release,
    CEcoLab1_Multiplication,
    CEcoLab1_Division
};


/*
 *
 * <сводка>
 *   Функция Create
 * </сводка>
 *
 * <описание>
 *   Функция создания экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD createCEcoLab1(/* in */ IEcoUnknown* pIUnkSystem, /* in */ IEcoUnknown* pIUnkOuter, /* out */ IEcoLab1** ppIEcoLab1) {
    int16_t result = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    CEcoLab1* pCMe = 0;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;
	
    /* Проверка указателей */
    if (ppIEcoLab1 == 0 || pIUnkSystem == 0) {
        return result;
    }

    /* Получение системного интерфейса приложения */
    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem1, (void **)&pISys);

    /* Проверка */
    if (result != 0 && pISys == 0) {
        return result;
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

	/* Получение идентификатора компонента для работы с памятью */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Получение интерфейса распределителя памяти */
    pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 && pIMem == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
        pISys->pVTbl->Release(pISys);
        return result;
    }

    /* Выделение памяти для данных экземпляра */
    pCMe = (CEcoLab1*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoLab1));

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = pISys;

    /* Сохранение указателя на интерфейс для работы с памятью */
    pCMe->m_pIMem = pIMem;

    /* Установка счетчика ссылок на компонент */
    pCMe->m_cRef = 1;

    /* Создание таблицы функций интерфейса IEcoLab1 */
    pCMe->m_pVTblIEcoLab1 = &g_x277FC00C35624096AFCFC125B94EEC90VTbl;

    /* Создание таблицы функций интерфейса IEcoConnectionPointContainer */
    pCMe->m_pVTblICPC = &g_x0000000500000000C000000000000046VTblCPC;

	pCMe->m_pVTblIEcoCalculatorX = &g_x9322111622484742AE0682819447843DVTblCX;
    pCMe->m_pVTblIEcoCalculatorY = &g_xBD6414C29096423EA90C04D77AFD1CADVTblCY;
   
    /* Инициализация данных */
    pCMe->m_Name = 0;

    /* Возврат указателя на интерфейс */
    *ppIEcoLab1 = (IEcoLab1*)pCMe;

    /* Освобождение */
    pIBus->pVTbl->Release(pIBus);

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Delete
 * </сводка>
 *
 * <описание>
 *   Функция освобождения экземпляра
 * </описание>
 *
 */
void ECOCALLMETHOD deleteCEcoLab1(/* in */ IEcoLab1* pIEcoLab1) {
    CEcoLab1* pCMe = (CEcoLab1*)pIEcoLab1;
    IEcoMemoryAllocator1* pIMem = 0;

    if (pIEcoLab1 != 0 ) {
        pIMem = pCMe->m_pIMem;
        /* Освобождение */
        if ( pCMe->m_Name != 0 ) {
            pIMem->pVTbl->Free(pIMem, pCMe->m_Name);
        }
        if ( pCMe->m_pISys != 0 ) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}
