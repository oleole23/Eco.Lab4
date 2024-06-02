/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include "IdEcoList1.h"
#include "CEcoLab1Sink.h"
#include "IEcoConnectionPointContainer.h"
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
void PrintProcessBefore(int n,  Process processes[]){
	int i;

    for (i = 0; i < n; i++) {
        printf("Process %d:\n", processes[i].id);
		printf("Arrival time: %d  Burst time: %d\n", processes[i].arrival_time, processes[i].burst_time);
    }
    printf("\n");
}

void PrintProcessCompletion(int n,  int *completion_order){
	int i;

	printf("The order of completion of the processes by id:\n");
    for (i = 0; i < n; i++) {
        printf("%d ", completion_order[i]);
    }
    printf("\n\n");
}

void PrintProcessAfter(int n,  Process processes[]){
	int i;

    for (i = 0; i < n; i++) {
        printf("Process %d:\n", processes[i].id);
		printf("Waiting time: %d  Completion time: %d\n", processes[i].waiting_time, processes[i].completion_time);
    }
    printf("\n");
}

void AverageWaitingTime(int n,  Process processes[]){
	int i = 0;
	int sum_waiting = 0;
	float average_waiting;

	for (i = 0; i < n; i++) {
        sum_waiting = sum_waiting + processes[i].waiting_time;
	}

	average_waiting = sum_waiting / (float)n;
	printf("Average waiting time: %.2f\n", average_waiting);
}

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    /* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;

	 /* Указатель на интерфейс контейнера точек подключения */
    IEcoConnectionPointContainer* pICPC = 0;
    /* Указатель на интерфейс точки подключения */
    IEcoConnectionPoint* pICP = 0;
    /* Указатель на обратный интерфейс */
    IEcoLab1Events* pIEcoLab1Sink = 0;
    IEcoUnknown* pISinkUnk = 0;
    uint32_t cAdvise = 0;

    IEcoCalculatorX* pIEcoCalculatorX = 0;
    IEcoCalculatorY* pIEcoCalculatorY = 0;

	int completion_order[100];
	int i;
	Process processes[] = {
        {1, 0, 3},
        {2, 6, 6},
        {3, 4, 4},
        {4, 2, 5},
        {5, 8, 2}
    };
	int n = sizeof(processes) / sizeof(processes[0]);

    /* Проверка и создание системного интрефейса */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem1, (void **)&pISys);
        if (result != 0 && pISys == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
            goto Release;
        }
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
#ifdef ECO_LIB
    /* Регистрация статического компонента для работы со списком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы со списком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoList1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_53884AFC93C448ECAA929C8D3A562281);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorA, (IEcoUnknown*) GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E);
    if (result != 0) {
        goto Release;
    }

    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorB, (IEcoUnknown*) GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565);
    if (result != 0) {
        goto Release;
    }

    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorD, (IEcoUnknown*) GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61);
    if (result != 0) {
        goto Release;
    }

    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorE, (IEcoUnknown*) GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0);
    if (result != 0) {
        goto Release;
    }
#endif
    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 || pIMem == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
        goto Release;
    }

    /* Получение тестируемого интерфейса */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
	if (result != 0 || pIEcoLab1 == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }

	/* Проверка поддержки подключений обратного интерфейса */
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoConnectionPointContainer, (void **)&pICPC);
    if (result != 0 || pICPC == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }

    /*Запрос на получения интерфейса точки подключения*/
    result = pICPC->pVTbl->FindConnectionPoint(pICPC, &IID_IEcoLab1Events, &pICP);
	if (result != 0 || pICP == 0) {
        /* Освобождение интерфейсов в случае ошибки*/ 
        goto Release;
    }
    /* Освобождение интерфейса*/ 
    pICPC->pVTbl->Release(pICPC);

    /* Создание экземпляра обратного интерфейса*/ 
    result = createCEcoLab1Sink(pIMem, (IEcoLab1Events**)&pIEcoLab1Sink);

    if (pIEcoLab1Sink != 0) {
        result = pIEcoLab1Sink->pVTbl->QueryInterface(pIEcoLab1Sink, &IID_IEcoUnknown,(void **)&pISinkUnk);
        if (result != 0 || pISinkUnk == 0) {
            /* Освобождение интерфейсов в случае ошибки*/ 
            goto Release;
        }
        /* Подключение*/ 
        result = pICP->pVTbl->Advise(pICP, pISinkUnk, &cAdvise);
        /* Проверка*/ 
        if (result == 0 && cAdvise == 1) {
            /* Сюда можно добавить код*/ 
        }
        /* Освобождение интерфейса*/ 
        pISinkUnk->pVTbl->Release(pISinkUnk);
    }

	PrintProcessBefore(n, processes);
	pIEcoLab1->pVTbl->scheduleProcesses(pIEcoLab1, processes, n, completion_order);
	PrintProcessCompletion(n, completion_order);
	PrintProcessAfter(n, processes);

	AverageWaitingTime(n, processes);

    if (pIEcoLab1Sink != 0) {
        /* Отключение*/ 
        result = pICP->pVTbl->Unadvise(pICP, cAdvise);
        pIEcoLab1Sink->pVTbl->Release(pIEcoLab1Sink);
        pICP->pVTbl->Release(pICP);
    }

	result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**) &pIEcoCalculatorX);
    if (result != 0 || pIEcoCalculatorX == 0) {
        goto Release;
    }

    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**) &pIEcoCalculatorY);
    if (result != 0 || pIEcoCalculatorY == 0) {
        goto Release;
    }
	
	goto Release;

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) {
        pIEcoLab1->pVTbl->Release(pIEcoLab1);
    }


    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}

