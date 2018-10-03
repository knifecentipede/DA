#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

struct Line{
    short day;
    short month;
    short year;
    char date[12];
    char *txt;
 };

int main(int argc, char *argv[])
{
    Line *test, *test2;
    int i = 0,j,k,num,testsize = 100, *countarr;
    char temp[2051];
    test=(Line*)malloc(sizeof(Line)*testsize);
    while(scanf("%s\t%s",test[i].date,temp) == 2) {
        test[i].txt=(char*)malloc(sizeof(char)*(strlen(temp) + 1));
        strcpy (test[i].txt, temp);
        sscanf(test[i].date,"%hd.%hd.%hd",&test[i].day,&test[i].month,&test[i].year);
        i++;
        if (i == testsize) {
            testsize = testsize*2;
            test = (Line*)realloc(test,sizeof(Line)*testsize);
        }

    }
    num = i;
    test2 = (Line*)malloc(sizeof(Line)*num);

    countarr = (int*)malloc(sizeof(int)*100);

    // DAY SORT
    for (i = 0; i < 31; i++)
        countarr[i] = 0;
    for (i = 0; i < num; i++) {
        countarr[test[i].day-1]++;
    }
    for (i = 1; i < 31; i++){
        countarr[i] += countarr[i-1];
    }
    for (j = num-1; j >= 0; j--) {
        test2[countarr[test[j].day-1]-1]=test[j];
        countarr[test[j].day-1]--;
    }
    swap(test, test2);

    // MONTH SORT
    for (i = 0; i < 12; i++)
        countarr[i] = 0;
    for (i = 0; i < num; i++) {
        countarr[test[i].month-1]++;
    }
    for (i = 1; i < 12; i++){
        countarr[i] += countarr[i-1];
    }
    for (j = num-1; j >= 0; j--) {
        test2[countarr[test[j].month-1]-1]=test[j];
        countarr[test[j].month-1]--;
    }
    swap(test, test2);

    // YEAR SORT
    int spt1 = 100, spt2 = 1;
    for (k = 0; k <= 1; k++){
        for (i = 0; i <= 99; i++)
            countarr[i] = 0;
        for (i = 0; i < num; i++) {
            countarr[(test[i].year%spt1)/spt2]++;
        }
        for (i = 1; i <= 99; i++){
            countarr[i] += countarr[i-1];
        }
        for (j = num-1; j >= 0; j--) {
            test2[countarr[(test[j].year%spt1)/spt2]-1]=test[j];
            countarr[(test[j].year%spt1)/spt2]--;
        }
        spt1*=100;
        spt2*=100;
        swap(test, test2);
    }

    // OUTPUT
    for (i = 0;i < num;i++) {
        cout<<test[i].date<<"\t"<<test[i].txt;
        cout<<"\n";
    }
    for (j=0;i<num;i++)
        free(test[i].txt);
    free(test);
    free(test2);
    free(countarr);
    return 0;
}
