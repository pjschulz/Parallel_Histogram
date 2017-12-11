#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "omp.h"

int findBin(float d, float pDouble[], int count, float measurement);
float getMin(float*, int);
float getMax(float*, int);
void getRandomData(int dataCount, float *array);

int main()
{
    int dataCount = 50;  //Number of Measurements
    int binCount = 5;    //Number of bins in the graph
    int threadCount = 5; //Set number of threads
    srand(time(NULL));   //Seeding for random values
    float data[dataCount]; //Array of Data
    getRandomData(dataCount, data);
    float minMeasurement = getMin(data, dataCount);  //Min value in the array
    float maxMeasurement = getMax(data, dataCount);  //Max value in the array
    float binMaxes[binCount];     //Array to store max value per bin
    int binCounts[binCount];      //Array to store the number of hits in each bin range
    float binWidth = (maxMeasurement - minMeasurement) / binCount;  //For determining max values per bin

    int b;
    //parallelized for loop
    #pragma omp parallel for num_threads(threadCount) default(none) shared(minMeasurement, maxMeasurement, binMaxes, binCounts, binCount, binWidth) private(b)
    for (b = 0; b < binCount; b++)
    {
        binCounts[b] = 0.0f;  //Initializing elements within binCounts to 0
        binMaxes[b] = minMeasurement + binWidth * (b + 1);  //Calculating the binMaxes per bin
    }

    int i;
    int bin;
    #pragma omp parallel for num_threads(threadCount) default(none) shared(data, dataCount, minMeasurement, binMaxes, binCount, binCounts ) private(bin, i)
    for ( i = 0; i < dataCount; i++)
    {
        bin = findBin(data[i], binMaxes, binCount, minMeasurement); //returns the bin in which the data element belongs
        #pragma omp critical
        binCounts[bin]++;
    }

    int j;
    int c;
    #pragma omp parallel for num_threads(threadCount) default(none) shared(binWidth,binMaxes, binCount, binCounts) private(j, c)
    for (j = 0; j < binCount; j++)
    {
        #pragma omp critical
        printf("Between %f and %f: ", binMaxes[j] - binWidth, binMaxes[j]);

        for (c = 0; c < binCounts[j]; c++)
        {
            printf("X");
        }
        printf("\r\n");

    }
    return 0;
}

int findBin(float data, float binMaxes[], int binCount, float minMeasurement)
{
    for (int b = 0; b < binCount; b++)
    {
        //printf("%f : %f : %f\r\n", minMeasurement, data, binMaxes[b]);
        if ((minMeasurement <= data ) && (data <= binMaxes[b]))
        {
            //printf("%f : %f : %f\r\n", minMeasurement, data, binMaxes[b]);
            return b;
        }
    }
    return 0;
}

float randFrom(float min, float max)
{
    float range = (max - min);
    float div = RAND_MAX / range;
    return min + (rand() / div);
}

void  getRandomData(int numOfData, float *array)
{
    for (int i = 0; i < numOfData; i++)
    {
        array[i] = randFrom(0.0, 20.0);
        printf("%f\r\n", array[i]);
    }
}
float getMin(float* array, int dataCount)
{
    float minValue = array[0];
    for (int i = 0; i < dataCount; i++)
    {
        if (array[i] < minValue)
        {
            minValue = array[i];
        }
    }
    printf("Min Value: %f\r\n", minValue);
    return minValue;
}
float getMax(float* array, int dataCount)
{
    float maxValue = array[0];
    for (int i = 0; i < dataCount; i++)
    {
        if (array[i] > maxValue)
        {
            maxValue = array[i];
        }
    }
    printf("Max Value: %f\r\n", maxValue);
    return maxValue;
}

